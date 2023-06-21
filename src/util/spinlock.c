#include <stdatomic.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "spinlock.h"

void lock(Spinlock *spinlock)
{
    int ticket = atomic_fetch_add(&spinlock->ticket, 1);
    while (spinlock->turn != ticket)
    {}
}

void unlock(Spinlock *spinlock)
{
    atomic_fetch_add(&spinlock->turn, 1);
}

Spinlock *spinlock_init()
{
    Spinlock *spinlock = (Spinlock *)malloc(sizeof(Spinlock));
    if (spinlock == NULL)
    {
        printf("Failed to allocate memory for spinlock. \n");
        exit(0);
    }

    spinlock->lock = lock;
    spinlock->unlock = unlock;

    atomic_init(&spinlock->ticket, 0);
    atomic_init(&spinlock->turn, 0);

    return spinlock;
}
