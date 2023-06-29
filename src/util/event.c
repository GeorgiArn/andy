#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>

#include "event.h"

static size_t g_max_fds = 0;

static void add(EventsSystem *es, int fd, uint32_t events)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = events;

    if (epoll_ctl(es->epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1)
    {
        printf("Failed to add event. \n");
        exit(0);
    }
}

static void mod(EventsSystem *es, int fd, uint32_t events)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = events;

    if (epoll_ctl(es->epoll_fd, EPOLL_CTL_MOD, fd, &event) == -1)
    {
        printf("Failed to modify event. \n");
        exit(0);
    }
}

static void del(EventsSystem *es, int fd)
{
    if (epoll_ctl(es->epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1)
    {
        printf("Failed to delete event. \n");
        exit(0);
    } 
}

static size_t wait(EventsSystem *es)
{
    size_t nready = epoll_wait(es->epoll_fd, es->incoming_events, g_max_fds, -1);
    return nready;
}

EventsSystem *events_system_init(size_t max_fds)
{
    EventsSystem *es = (EventsSystem*) malloc(sizeof(EventsSystem));
    if (es == NULL)
    {
        printf("Failed to allocate memory for events system.\n");
        exit(0);
    }

    int fd = epoll_create1(0);

    es->epoll_fd = fd;
    es->incoming_events = calloc(max_fds, sizeof(struct epoll_event));

    es->add = add;
    es->mod = mod;
    es->del = del;
    es->wait = wait;

    g_max_fds = max_fds;
    
    return es;
}