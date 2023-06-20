#include <stdlib.h>
#include <stdio.h>

#include "shm.h"

// TODO
static void alloc(size_t bytes)
{
}

// TODO
static void *alloc_pages(unsigned int pg_count)
{
}

// TODO
static void *free_pages(void *addr, unsigned int pg_count)
{
}

SharedMemory* shm_init() 
{
    SharedMemory* shm = (SharedMemory*) malloc(sizeof(SharedMemory));
    if (shm == NULL)
    {
        printf("Failed to allocate memory for shared memory interface. \n");
        exit(0);
    }

    shm->alloc = alloc;
    shm->alloc_pages = alloc_pages;
    shm->free_pages = free_pages;

    return shm;
}