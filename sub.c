//
// Created by Lucas Tiedtke on 26.03.22.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/sem.h>

#include "sub.h"
#include "commandParser.h"
#include "keyValueStore.h"
#include "socket.h"

void error_exit(char *error_message) {
    fprintf(stderr, "%s: %s\n", error_message, strerror(errno));
    exit(EXIT_FAILURE);
}

struct sembuf pp = { 0, -1, SEM_UNDO };
struct sembuf vv = { 0, +1, SEM_UNDO };

void execute(char *buffer, socket_t client, int semid) {
    command *cmd = malloc(sizeof(command));
    char cmd_buffer[64];

    FILE  *shell;
    char shell_buffer[64];

    parse(buffer, cmd);
    memset(buffer, 0, strlen(buffer));

    switch (cmd->op) {
        case GET:
            if (safeget(cmd->key, cmd_buffer, semid) < 0)
                strcpy(cmd_buffer, "key_nonexistent");

            strcpy(buffer, "GET");
            strcat(buffer, ":");
            strcat(buffer, cmd->key);
            strcat(buffer, ":");
            strcat(buffer, cmd_buffer);
            strcat(buffer, "\n");
            send_socket(&client, buffer, strlen(buffer));
            break;
        case PUT:
            if (safeput(cmd->key, cmd->value, semid) < 0)
                strcpy(cmd_buffer, "datastore_nospace");

            strcpy(buffer, "PUT");
            strcat(buffer, ":");
            strcat(buffer, cmd->key);
            strcat(buffer, ":");
            strcat(buffer, cmd->value);
            strcat(buffer, "\n");
            send_socket(&client, buffer, strlen(buffer));
            break;
        case DEL:
            if (safedel(cmd->key, semid) < 0)
                strcpy(cmd_buffer, "key_nonexistent");
            else
                strcpy(cmd_buffer, "key_deleted");

            strcpy(buffer, "DEL");
            strcat(buffer, ":");
            strcat(buffer, cmd->key);
            strcat(buffer, ":");
            strcat(buffer, cmd_buffer);
            strcat(buffer, "\n");
            send_socket(&client, buffer, strlen(buffer));
            break;
        case QUIT:
            strcpy(buffer, "QUIT");
            break;
        case BEG:
            semop(semid, &pp, 1);
            break;
        case END:
            semop(semid, &vv, 1);
            break;
        case SUB:
            if (safeget(cmd->key, cmd_buffer, semid) < 0)
                strcpy(cmd_buffer, "key_nonexistent");
            else
                addsub(cmd->key, client);

            strcpy(buffer, "SUB");
            strcat(buffer, ":");
            strcat(buffer, cmd->key);
            strcat(buffer, ":");
            strcat(buffer, cmd_buffer);
            strcat(buffer, "\n");
            send_socket(&client, buffer, strlen(buffer));
            break;
        case OP:
            if (safeget(cmd->key, cmd_buffer, semid) < 0)
                strcpy(cmd_buffer, "key_nonexistent");
            else {
                strcpy(shell_buffer, cmd->value);
                strcat(shell_buffer, " ");
                strcat(shell_buffer, cmd_buffer);
                if ((shell = popen(shell_buffer, "r")) == NULL)
                    fprintf(stderr, "Failed to execute shell command");
                else {
                    fgets(shell_buffer, sizeof(shell_buffer), shell);
                    if (shell_buffer[strlen(shell_buffer)-1] == '\n')
                        shell_buffer[strlen(shell_buffer)-1] = '\0';
                    safeput(cmd->key, shell_buffer, semid);
                }
            }
            strcpy(buffer, "OP");
            strcat(buffer, ":");
            strcat(buffer, cmd->key);
            strcat(buffer, ":");
            strcat(buffer, cmd_buffer);
            strcat(buffer, "\n");
            send_socket(&client, buffer, strlen(buffer));
            break;
        case INV:
            strcpy(buffer, "invalid command");
            strcat(buffer, "\n");
            send_socket(&client, buffer, strlen(buffer));
            break;
    }

    free(cmd->key);
    free(cmd->value);
//    free(cmd);
}

void sighandler(int sig, sighandler_t handler) {
    struct sigaction new_action, old_action;
    new_action.sa_handler = handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = SA_RESTART;
    sigaction(sig, &new_action, &old_action);
}

void wait_children() {
    pid_t pid;
    do {
        pid = waitpid(-1, NULL, WNOHANG);
    } while (pid > 0);
}