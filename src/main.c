#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "conf.h"
#include "shm.h"
#include "server.h"

int main(int argc, char **argv)
{
    ServerConfig *config = server_conf_init(CONF_FILE);
    SharedMemory *shm = shared_memory_init();
    TCPServer *server = tcp_server_init(); 

    server->start(server, config);
    printf("%d\n", server->fd);
    server->stop(server);

    exit(0);
}