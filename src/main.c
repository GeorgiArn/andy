#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "conf.h"
#include "shm.h"
#include "server.h"
#include "master.h"

int main(int argc, char **argv)
{
    ServerConfig *config = server_conf_init(CONF_FILE);
    MasterProcess *master = master_process_init(config);

    master->run_ev_loop(master);

    exit(0);
}