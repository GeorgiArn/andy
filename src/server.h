#ifndef SERVER_H
#define SERVER_H

#include "conf.h"

typedef struct TCPServer {
    int fd;
    char* host;
    void (*start)(struct TCPServer *server, ServerConfig *config);
    void (*stop)(struct TCPServer *server);
} TCPServer;

TCPServer *tcp_server_init();

#endif
