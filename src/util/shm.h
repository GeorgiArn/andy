#ifndef SHM_H
#define SHM_H

typedef struct SharedMemory
{
    void *(*alloc)(size_t bytes);
} SharedMemory;

SharedMemory* shared_memory_init();

#endif