//
// Created by Lucas Tiedtke on 27.03.22.
//

#include <string.h>

#include "commandParser.h"

int parseValue(char *s, command *cmd, char **save_ptr) {
    cmd->value = strdup(strtok_r(s, "", save_ptr));
    return cmd->value == NULL;
}

int parseKey(char *s, command *cmd, char **save_ptr) {
    cmd->key = strdup(strtok_r(s, " ", save_ptr));
    return cmd->key == NULL;
}

int parseOperation(char *s, command *cmd) {
    int ret = 0;
    char *op;

    op = strtok_r(s, " ", &s);

    if (op == NULL) {
        cmd->op = INV;
        ret = 1;
    } else if (strcmp(op, "GET") == 0) {
        cmd->op = GET;
        ret = parseKey(s, cmd, &s);
    } else if (strcmp(op, "PUT") == 0) {
        cmd->op = PUT;
        ret = parseKey(s, cmd, &s) || parseValue(s, cmd, &s);
    } else if (strcmp(op, "DEL") == 0) {
        cmd->op = DEL;
        ret = parseKey(s, cmd, &s);
    } else if (strcmp(op, "QUIT") == 0) {
        cmd->op = QUIT;
    } else if (strcmp(op, "BEG") == 0) {
        cmd->op = BEG;
    } else if (strcmp(op, "END") == 0) {
        cmd->op = END;
        ret = parseKey(s, cmd, &s);
    } else if (strcmp(op, "SUB") == 0) {
        cmd->op = SUB;
        ret = parseKey(s, cmd, &s);
    } else if (strcmp(op, "OP") == 0) {
        cmd->op = OP;
        ret = parseKey(s, cmd, &s) || parseValue(s, cmd, &s);
    } else { // invalid operation
        cmd->op = INV;
        ret = 1;
    }

    if (s != NULL) { // too many arguments
        ret = 1;
    }
    if (ret != 0) {
        cmd->op = INV;
    }
    return ret;
}

int parse(char *s, command *cmd) {
    return parseOperation(s, cmd);
}