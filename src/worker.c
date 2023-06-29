#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <errno.h>

#include "worker.h"
#include "event.h"

#define MAXFDS 1024

static void run_ev_loop(WorkerProcess *worker, TCPServer *server)
{
    printf("Worker %d created. \n", worker->pid);

    EventsSystem *es = events_system_init(MAXFDS);
    es->add(es, server->fd, EPOLLIN);

    while (1)
    {
        size_t nready = es->wait(es);

        for (size_t i = 0; i < nready; i++)
        {
            if (es->incoming_events[i].data.fd == server->fd)
            {
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
                    usleep(200000);
            }
            else
            {
                int client_fd = es->incoming_events[i].data.fd;

                if (es->incoming_events[i].events & EPOLLIN)
                {
                    char buffer[1024];
                    int nbytes = recv(client_fd, buffer, sizeof(buffer), 0);

                    if (nbytes == 0)
                    {
                        // client disconnected
                        es->del(es, client_fd);
                        close(client_fd);
                        continue;
                    }

                    if (nbytes < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
                        es->mod(es, client_fd, EPOLLIN);
                    else if (nbytes > 0)
                        es->mod(es, client_fd, EPOLLOUT);
                }
                else if (es->incoming_events[i].events & EPOLLOUT)
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
            }
        }
    }
}

WorkerProcess *worker_process_init(MasterProcess *master)
{
    WorkerProcess *worker = (WorkerProcess *)malloc(sizeof(WorkerProcess));
    if (worker == NULL)
    {
        printf("Failed to allocate memory for worker process. \n");
        exit(0);
    }

    worker->pid = -1;
    worker->cpuid = -1;
    worker->run_ev_loop = run_ev_loop;

    return worker;
}