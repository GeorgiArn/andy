#ifndef MASTER_H
#define MASTER_H

#include <unistd.h>

#include "conf.h"
#include "system.h"

typedef struct MasterProcess
{
    pid_t pid;
    size_t workers_count;
    void (*run_ev_loop)(struct MasterProcess *master);
} MasterProcess;

MasterProcess *master_process_init(System *system, ServerConfig *config);

#endif