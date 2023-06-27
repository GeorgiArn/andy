#ifndef SERVER_H
#define SERVER_H

#include "conf.h"
#include "spinlock.h"
#include "shm.h"

typedef struct TCPServer {
    int fd;
    char* host;
    int port;
    Spinlock *g_accept_lock;
    void (*start)(struct TCPServer *server, ServerConfig *config);
    void (*stop)(struct TCPServer *server);
} TCPServer;

TCPServer *tcp_server_init(SharedMemory *shm);

#endif
