#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <errno.h>

#include "worker.h"

#define MAXFDS 1024

// TODO: Abstract tasks, work on error handling
static void run_ev_loop(WorkerProcess *worker, TCPServer *server)
{
    printf("Worker %d created. \n", worker->pid);

    int epoll_fd = epoll_create1(0);

    struct epoll_event accept_event;
    accept_event.data.fd = server->fd;
    accept_event.events = EPOLLIN;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server->fd, &accept_event);

    struct epoll_event *events = calloc(MAXFDS, sizeof(struct epoll_event));

    while (1)
    {
        size_t nready = epoll_wait(epoll_fd, events, MAXFDS, -1);

        for (size_t i = 0; i < nready; i++)
        {
            if (events[i].data.fd == server->fd)
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

                        struct epoll_event read_event = {0};
                        read_event.data.fd = client_fd;
                        read_event.events |= EPOLLIN;

                        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &read_event);
                    }
                    server->g_accept_lock->unlock(server->g_accept_lock);
                }
                else
                    usleep(200000);
            }
            else
            {
                int client_fd = events[i].data.fd;

                struct epoll_event event = {0};
                event.data.fd = client_fd;

                if (events[i].events & EPOLLIN)
                {
                    char buffer[1024];
                    int nbytes = recv(client_fd, buffer, sizeof(buffer), 0);

                    if (nbytes == 0)
                    {
                        // client disconnected
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                        close(client_fd);
                        continue;
                    }

                    if (nbytes < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
                        event.events |= EPOLLIN;
                    else if (nbytes > 0)
                        event.events |= EPOLLOUT;

                    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &event);
                }
                else if (events[i].events & EPOLLOUT)
                {
                    char buffer[1024] = "HTTP/1.1 200 OK\r\n\r\nyooo";
                    int nbytes = send(client_fd, buffer, sizeof(buffer), 0);

                    if (nbytes < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
                        event.events |= EPOLLOUT;
                    else if (nbytes >= 0)
                    {
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
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