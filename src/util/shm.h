#ifndef SHM_H
#define SHM_H

#include <unistd.h>

#include <system.h>

typedef struct SharedMemory
{
    void *(*alloc)(size_t bytes);
} SharedMemory;

SharedMemory* shared_memory_init(System *system);

#endif