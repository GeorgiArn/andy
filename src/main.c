#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "conf.h"
#include "master.h"
#include "system.h"

int main(int argc, char **argv)
{
    System *system = system_init();
    ServerConfig *config = server_conf_init(CONF_FILE);

    MasterProcess *master = master_process_init(system, config);

    printf("Master process id: %d\n", master->pid);

    master->run_ev_loop(master);

    exit(0);
}