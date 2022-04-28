//
// Created by Lucas Tiedtke on 28.04.22.
//

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define socket_t int
#define BUF 1024

int main() {
    socket_t server, client;
    const int opt = 1;
    struct sockaddr_in addr;
    unsigned int len;
    char *buffer;

    // create
    server = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // bind
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(5678);
    bind(server, (struct sockaddr *) &addr, sizeof(addr));

    // listen
    listen(server, 1);

    // accept
    len = sizeof(addr);
    client = accept(server, (struct sockaddr *)&addr, &len);

    // allocate buffer
    buffer = (char *) malloc(BUF);

    // run loop
    while (1) {
        send(client, ">>> ", 4, 0);
        recv(client, buffer, BUF - 1, 0);
        send(client, buffer, strlen(buffer), 0);
        memset(buffer, 0, strlen(buffer));
    }
}