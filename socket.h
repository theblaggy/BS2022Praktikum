//
// Created by Lucas Tiedtke on 27.03.22.
//

#ifndef PRAKTIKUM_SOCKET_H
#define PRAKTIKUM_SOCKET_H

#endif //PRAKTIKUM_SOCKET_H

#define socket_t int

int create_socket();
void bind_socket(const socket_t *sock);
void listen_socket(const socket_t *sock);
void accept_socket(const socket_t *sock, socket_t *new_sock);
void close_socket(const socket_t *sock);
void receive_socket(const socket_t *sock, char *data, size_t size);
void send_socket(const socket_t *sock, char *data, size_t size);
