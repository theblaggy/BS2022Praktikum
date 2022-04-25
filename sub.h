//
// Created by Lucas Tiedtke on 26.03.22.
//

#include "socket.h"

#ifndef PRAKTIKUM_SUB_H
#define PRAKTIKUM_SUB_H

#endif //PRAKTIKUM_SUB_H

typedef void (*sighandler_t)(int);

void error_exit(char *error_message);
void execute(char *command, socket_t client, int semid);
void sighandler(int sig, sighandler_t handler);
void wait_children();
