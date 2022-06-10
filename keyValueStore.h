//
// Created by Lucas Tiedtke on 26.03.22.
//

#include "socket.h"

#ifndef PRAKTIKUM_KEYVALUESTORE_H
#define PRAKTIKUM_KEYVALUESTORE_H

#endif //PRAKTIKUM_KEYVALUESTORE_H

int init_datastore();
int deinit_datastore();
int safeput(char *key, char *value, int semid);
int safeget(char *key, char *res, int semid);
int safedel(char *key, int semid);
int addsub(char *key, socket_t sub);
