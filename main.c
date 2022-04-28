//
// Created by Lucas Tiedtke on 28.04.22.
//

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF 1024

int main() {

    int server = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    const int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(5678);
    bind(server, (struct sockaddr *) &addr, sizeof(addr));

    listen(server, 1);

    unsigned int len = sizeof(addr);
    int client = accept(server, (struct sockaddr *)&addr, &len);

    char *buffer = (char *) malloc(BUF);

    while (1) {
        send(client, ">>> ", 4, 0);
        recv(client, buffer, BUF - 1, 0);
        send(client, buffer, strlen(buffer), 0);
        memset(buffer, 0, strlen(buffer));
    }
}