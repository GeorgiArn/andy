#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "server.h"

static int get_port(ServerConfig *config)
{
    const char *port_str = config->get_entry("port");

    if (port_str == NULL)
    {
        printf("Server port must be provided in %s. \n", config->filename);
        exit(0);
    }

    int port = atoi(port_str);
    if (port <= 0)
    {
        printf("Invalid server port in %s. \n", config->filename);
        exit(0);
    }

    return port;
}

static void set_nonblock(int socket_fd)
{
    int flags = fcntl(socket_fd, F_GETFL, 0);
    if (fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) != 0)
    {
        printf("Failed to set server to non-blocking mode");
        exit(0);
    }
}

static void start(TCPServer *server, ServerConfig *config)
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        printf("Failed to create server socket. \n");
        exit(0);
    }

    set_nonblock(socket_fd);

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(get_port(config));
    // TODO: Set server address to the one specified in andy.conf
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_fd, (struct sockaddr *)&address, sizeof(address)) == -1)
    {
        printf("Failed to bind server socket. \n");
        exit(0);
    }

    if (listen(socket_fd, SOMAXCONN) == -1)
    {
        printf("Failed to listen for connections. \n");
        exit(0);
    }

    server->fd = socket_fd;
}

static void stop(TCPServer *server)
{
    if (server->fd == 0)
    {
        printf("Failed to stop server. \n");
        exit(0);
    }

    close(server->fd);
}

TCPServer *tcp_server_init()
{
    TCPServer *server = (TCPServer *)malloc(sizeof(TCPServer));
    if (server == NULL)
    {
        printf("Failed to allocate memory for tcp server.");
        exit(0);
    }

    server->fd = 0;
    server->start = start;
    server->stop = stop;

    return server;
}