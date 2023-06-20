#ifndef SHM_H
#define SHM_H

typedef struct SharedMemory
{
    void *(*alloc)(size_t bytes);
    void *(*alloc_pages)(unsigned int pg_count);
    void (*free_pages)(void *addr, unsigned int pg_count);
} SharedMemory;

SharedMemory* shm_init();

#endif