#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

#include "shm.h"
#include "spinlock.h"

#define MEM_ALIGN sizeof(unsigned long)

// Round up x to the nearest multiple of y
#define ROUND_UP(x, y) (((x + (y - 1)) / y) * y)

static System *g_system = NULL;

typedef struct shm
{
    char *addr;
    size_t size, offset;
    Spinlock spinlock;
} shm;

static shm *shm_obj = NULL;

static void *alloc(size_t bytes)
{
    bytes = ROUND_UP(bytes, MEM_ALIGN);

    shm_obj->spinlock.lock(&shm_obj->spinlock);
    if (bytes > shm_obj->size - bytes)
    {
        printf("Not enough space in shared memory to allocate %ld bytes. \n", bytes);
        exit(0);
    }

    void *addr = shm_obj->addr + shm_obj->offset;
    shm_obj->offset += bytes;
    shm_obj->spinlock.unlock(&shm_obj->spinlock);

    return addr;
}

static void *alloc_pages(unsigned int pg_count)
{
    void *addr = mmap(NULL, pg_count * g_system->page_size, PROT_READ | PROT_WRITE,
                      MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (addr == NULL)
    {
        printf("Failed to allocate shared memory. \n");
        exit(0);
    }

    return addr;
}

static void free_pages(void *addr, unsigned int pg_count)
{
    munmap(addr, pg_count * g_system->page_size);
}

static void shm_init()
{
    char *addr = (char *)alloc_pages(1);

    shm_obj = (shm *)addr;
    shm_obj->addr = addr;
    shm_obj->size = g_system->page_size;
    shm_obj->offset = ROUND_UP(sizeof(shm_obj), sizeof(MEM_ALIGN));
    shm_obj->spinlock = *spinlock_init();
}

SharedMemory *shared_memory_init(System *system)
{
    SharedMemory *shared_memory = (SharedMemory *)malloc(sizeof(SharedMemory));
    if (shared_memory == NULL)
    {
        printf("Failed to allocate memory for shared memory interface. \n");
        exit(0);
    }

    shared_memory->alloc = alloc;
    g_system = system;

    shm_init();

    return shared_memory;
}