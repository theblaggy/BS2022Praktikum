//
// Created by Lucas Tiedtke on 26.03.22.
//

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <search.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>

#include "keyValueStore.h"
#include "socket.h"
#include "sub.h"

#define DS 1024

typedef struct {
    int msqid;
    socket_t client;
} sub;

typedef struct {
    char key[64];
    char value[64];
    sub sub;
} item;

item *datastore;

struct message {
    long int type;
    char text[64];
};

struct sembuf p = { 0, -1, SEM_UNDO };
struct sembuf v = { 0, +1, SEM_UNDO };

int init_datastore() {
    int shmid;
    void *shmdata;

    shmid = shmget(IPC_PRIVATE, DS, IPC_CREAT | IPC_RMID | SHM_R | SHM_W);
    if (shmid == -1)
        fprintf(stderr, "Failed to create shared mem\n");

    shmdata = shmat(shmid, NULL, 0);
    if (shmdata == (void *) -1)
        fprintf(stderr, "Failed to attach shared mem\n");

    datastore = shmdata;
    memset(datastore, 0, DS);

    return 0;
}

int deinit_datastore() {

    if (shmdt(datastore))
        fprintf(stderr, "Failed to detach shared mem");

    return 0;
}

int put(char *key, char *value) {
//    ENTRY e = {key, value};
//    ENTRY *ep;
//
//    if ((ep = hsearch(e, FIND)) != NULL) {
//        ep->data = value;
//        return -1;
//    }
//    if (hsearch(e, ENTER) == NULL) {
//        fprintf(stderr, "Entry failed\n");
//        return 1;
//    }

    int i;
    for (i = 0; i < DS / sizeof(ENTRY); i++) { // search key to overwrite
        if (strcmp(datastore[i].key, key) == 0) {
            strcpy(datastore[i].value, value);

            if (datastore[i].sub.msqid != -1 && datastore[i].sub.client != -1) {
                struct message msg;
                msg.type = 1;
                strcpy(msg.text, "PUT");
                strcat(msg.text, ":");
                strcat(msg.text, datastore[i].key);
                strcat(msg.text, ":");
                strcat(msg.text, datastore[i].value);
                strcat(msg.text, "\n");
                if (msgsnd(datastore[i].sub.msqid, &msg, sizeof(msg.text), 0) < 0)
                    error_exit("Failed to send message");
            }

            return 1; // updated old
        }
    }
    for (i = 0; i < DS / sizeof(ENTRY); i++) { // search empty value to overwrite
        if (datastore[i].value[0] == '\0') {
            strcpy(datastore[i].key, key);
            strcpy(datastore[i].value, value);
            datastore[i].sub.msqid = -1;
            datastore[i].sub.client = -1;
            return 0; // added new
        }
    }
    return -1; // datastore filled, no space
}

int get(char *key, char *res) {
//    ENTRY e = {key};
//    ENTRY *ep;
//
//    if ((ep = hsearch(e, FIND)) == NULL || ep->data == NULL) {
//        return -1;
//    }
//    strcpy(res, (char *) ep->data);
    int i;
    for (i = 0; i < DS / sizeof(ENTRY); i++) {
        if (datastore[i].key[0] == '\0')
            return -1;
        if (strcmp(datastore[i].key, key) == 0 && datastore[i].value[0] != 0) {
            strcpy(res, (char *) datastore[i].value);
            break;
        }
    }
    return 0;
}

int del(char *key) {
//    ENTRY e = {key};
//    ENTRY *ep;
//
//    if ((ep = hsearch(e, FIND)) == NULL) {
//        return -1;
//    }
//    ep->data = (void *) NULL;
    int i;
    for (i = 0; i < DS / sizeof(ENTRY); i++) {
        if (datastore[i].key[0] == '\0')
            return -1;
        if (strcmp(datastore[i].key, key) == 0 && datastore[i].value[0] != '\0') {
            datastore[i].value[0] = '\0';

            if (datastore[i].sub.msqid != -1 && datastore[i].sub.client != -1) {
                struct message msg;
                msg.type = 1;
                strcpy(msg.text, "DEL");
                strcat(msg.text, ":");
                strcat(msg.text, datastore[i].key);
                strcat(msg.text, ":");
                strcat(msg.text, "key_deleted");
                strcat(msg.text, "\n");
                if (msgsnd(datastore[i].sub.msqid, &msg, sizeof(msg.text), 0) < 0)
                    error_exit("Failed to send message");
            }

            break;
        }
    }
    return 0;
}

int safeput(char *key, char *value, int semid) {
    int ret;
    int ta = semctl(semid, 0, GETVAL, NULL) == 0 && semctl(semid, 0, GETPID, NULL) == getpid();

    if (!ta)
        semop(semid, &p, 1);

    ret = put(key, value);

    if (!ta)
        semop(semid, &v, 1);
    return ret;
}

int safeget(char *key, char *res, int semid) {
    int ret;
    int ta = semctl(semid, 0, GETVAL, NULL) == 0 && semctl(semid, 0, GETPID, NULL) == getpid();

    if (!ta)
        semop(semid, &p, 1);

    ret = get(key, res);

    if (!ta)
        semop(semid, &v, 1);
    return ret;
}

int safedel(char *key, int semid) {
    int ret;
    int ta = semctl(semid, 0, GETVAL, NULL) == 0 && semctl(semid, 0, GETPID, NULL) == getpid();

    if (!ta)
        semop(semid, &p, 1);

    ret = del(key);

    if (!ta)
        semop(semid, &v, 1);
    return ret;
}

int addsub(char *key, socket_t client) {

    int i;
    for (i = 0; i < DS / sizeof(ENTRY); i++) {
        if (datastore[i].key[0] == '\0')
            return -1;
        if (strcmp(datastore[i].key, key) == 0 && datastore[i].value[0] != 0) {
            // add sub
            if ((datastore[i].sub.msqid = msgget(IPC_PRIVATE, IPC_CREAT | SHM_R | SHM_W)) < 0) {
                error_exit("Failed to create message queue");
            }
            datastore[i].sub.client = client;

            // wait to receive
            // send pub

            if (fork() == 0) {
                struct message msg;
                msg.type = 1;
                while (getppid() != 1) {
                    if (msgrcv(datastore[i].sub.msqid, &msg, sizeof(msg.text), 1, IPC_NOWAIT) >= 0) {
                        send_socket(&client, msg.text, strlen(msg.text));
                    }
                }
                close_socket(&client);
            }

            break;
        }
    }
    return 0;
}