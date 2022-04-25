//
// Created by Lucas Tiedtke on 27.03.22.
//

#ifndef PRAKTIKUM_COMMANDPARSER_H
#define PRAKTIKUM_COMMANDPARSER_H

#endif //PRAKTIKUM_COMMANDPARSER_H

enum operation { INV, GET, PUT, DEL, QUIT, BEG, END, SUB, OP };

typedef struct {
    enum operation op;
    char *key;
    char *value;
} command;

int parse(char *s, command *cmd);
