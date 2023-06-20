#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "conf.h"
#include "shm.h"

int main(int argc, char **argv)
{
    ServerConfig* config = server_conf_init(CONF_FILE);
    SharedMemory* shm = shared_memory_init();

    int *a = (int *)shm->alloc(sizeof(int));
    *a = 4;
    printf("Address: %p, Value: %d \n", a, *a);

    exit(0);
}