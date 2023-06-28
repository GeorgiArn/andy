#ifndef EVENT_H
#define EVENT_H

#include <sys/epoll.h>

typedef struct EventsSystem
{
    int epoll_fd;
    struct epoll_event *incoming_events;
    void (*add)(struct EventsSystem *es, int fd, uint32_t events);
    void (*mod)(struct EventsSystem *es, int fd, uint32_t events);
    void (*del)(struct EventsSystem *es, int fd);
    size_t (*wait)(struct EventsSystem *es);
} EventsSystem;

EventsSystem *events_system_init();

#endif