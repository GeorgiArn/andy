#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "conf.h"
#include "shm.h"

int main(int argc, char **argv)
{
    ServerConfig* config = server_conf_init(CONF_FILE);
    SharedMemory* shm = shm_init();

    exit(0);
}