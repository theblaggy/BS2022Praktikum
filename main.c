//
// Created by Lucas Tiedtke on 26.03.22.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "keyValueStore.h"
#include "socket.h"
#include "sub.h"

#define BUF 1024

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main() {
    socket_t server, client;
    char *buffer = (char *) malloc(BUF);

    int semid;
    union semun sunion;

    semid = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_RMID | SHM_R | SHM_W);
    if (semid == -1)
        fprintf(stderr, "Failed to create sem");

    sunion.val = 1;
    if (semctl(IPC_PRIVATE, 0, SETVAL, sunion) < 0)
        fprintf(stderr, "Failed to init sem");

    init_datastore();

    server = create_socket();
    bind_socket(&server);
    listen_socket(&server);

    sighandler(SIGCHLD, wait_children);

    do {
        accept_socket(&server, &client);
        if (fork() == 0) { // inside child process
            close_socket(&server);
            do {
                send_socket(&client, ">>> ", 4);
                receive_socket(&client, buffer, BUF - 1);
                execute(buffer, client, semid);
            } while (strcmp(buffer, "QUIT") != 0);
            close_socket(&client);
        }
        close_socket(&client);
    } while (strcmp(buffer, "\n") != 0);

    close_socket(&server);
    deinit_datastore();
    free(buffer);
    return EXIT_SUCCESS;
}