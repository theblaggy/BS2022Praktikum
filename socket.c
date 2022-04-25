//
// Created by Lucas Tiedtke on 27.03.22.
//

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <semaphore.h>

#include "sub.h"
#include "socket.h"

int create_socket() {
    socket_t sock;
    const int opt = 1;

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0)
        error_exit("Failed to create socket");

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    return sock;
}

void bind_socket(const socket_t *sock) {
    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(5678);

    if (bind(*sock, (struct sockaddr *) &server, sizeof(server)) < 0)
        error_exit("Failed to bind socket");
}

void listen_socket(const socket_t *sock) {
    if (listen(*sock, 10) == -1)
        error_exit("Failed to set listening state");
}

void accept_socket(const socket_t *sock, socket_t *new_sock) {
    struct sockaddr_in client;
    unsigned int len;

    len = sizeof(client);
    if ((*new_sock = accept(*sock, (struct sockaddr *)&client, &len)) == -1)
        error_exit("Failed to accept connection");
}

void close_socket(const socket_t *sock) {
    close(*sock);
}

void receive_socket(const socket_t *sock, char *data, size_t size) {
    unsigned int len;
    len = recv(*sock, data, size, 0);
    if (len >= 0)
        data[len-2] = '\0';
    else
        error_exit("Failed to receive data");
}

void send_socket(const socket_t *sock, char *data, size_t size) {
    if (send(*sock, data, size, 0) == -1) {
        error_exit("Failed to send data");
    }
}