#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <errno.h>

#include "worker.h"
#include "event.h"

static size_t get_max_fds(ServerConfig *conf)
{
    const char *worker_connections_str = conf->get_entry("worker_connections");
    if (worker_connections_str == NULL)
    {
        printf("Number of worker connections must be specified in %s.\n", conf->filename);
        exit(0);
    }

    size_t worker_connections = strtoul(worker_connections_str, NULL, 10);
    if (worker_connections <= 0)
    {
        printf("Invalid number of worker connections in %s.\n", conf->filename);
        exit(0);
    }
    
    return worker_connections;
}

static void accept_client(EventsSystem *es, TCPServer *server)
{
    #define TIMEOUT 200000 // 0.2 seconds

    if (server->g_accept_lock->try_lock(server->g_accept_lock))
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        int client_fd = accept(server->fd, (struct sockaddr *)&client_addr, &client_addr_len);

        if (client_fd < 0 && (errno == EINTR || errno == EAGAIN || EWOULDBLOCK))
            printf("Accept returned EAGAIN or EWOULDBLOCK or EINTR. \n");
        else if (client_fd >= 0)
        {
            make_non_blocking(client_fd);
            es->add(es, client_fd, EPOLLIN);
        }
        server->g_accept_lock->unlock(server->g_accept_lock);
    }
    else
        usleep(TIMEOUT);
}

static void receive_from_client(EventsSystem *es, int client_fd)
{
    char buffer[1024];
    int nbytes = recv(client_fd, buffer, sizeof(buffer), 0);

    if (nbytes == 0)
    {
        // client disconnected
        es->del(es, client_fd);
        close(client_fd);
    }
    else if (nbytes < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
        es->mod(es, client_fd, EPOLLIN);
    else if (nbytes > 0)
        es->mod(es, client_fd, EPOLLOUT);
}

static void send_to_client(EventsSystem *es, int client_fd)
{
    char buffer[1024] = "HTTP/1.1 200 OK\r\n\r\nyooo";
    int nbytes = send(client_fd, buffer, sizeof(buffer), 0);

    if (nbytes < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
        es->mod(es, client_fd, EPOLLOUT);
    else if (nbytes >= 0)
    {
        es->del(es, client_fd);
        close(client_fd);
    }
}

static void run_ev_loop(WorkerProcess *worker, TCPServer *server)
{
    printf("Worker %d created. \n", worker->pid);

    EventsSystem *es = events_system_init(worker->max_fds);
    es->add(es, server->fd, EPOLLIN);

    while (true)
    {
        size_t nready = es->wait(es);

        for (size_t i = 0; i < nready; i++)
        {
            int fd = es->incoming_events[i].data.fd;
            uint32_t events = es->incoming_events[i].events;

            if (fd == server->fd)
                accept_client(es, server);
            else if (events & EPOLLIN)
                receive_from_client(es, fd);
            else if (events & EPOLLOUT)
                send_to_client(es, fd);
        }
    }
}

WorkerProcess *worker_process_init(MasterProcess *master, ServerConfig *conf)
{
    WorkerProcess *worker = (WorkerProcess *)malloc(sizeof(WorkerProcess));
    if (worker == NULL)
    {
        printf("Failed to allocate memory for worker process. \n");
        exit(0);
    }

    worker->pid = -1;
    worker->cpuid = -1;
    worker->max_fds = get_max_fds(conf);
    worker->run_ev_loop = run_ev_loop;

    return worker;
}