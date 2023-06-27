#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdatomic.h>

typedef struct Spinlock
{
    atomic_int ticket;
    atomic_int turn;

    void (*lock)(struct Spinlock *spinlock);
    int (*try_lock)(struct Spinlock *spinlock);
    void (*unlock)(struct Spinlock *spinlock);
} Spinlock;

Spinlock *spinlock_init();

#endif