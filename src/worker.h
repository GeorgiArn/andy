#ifndef WORKER_H
#define WORKER_H

#include "master.h"

typedef struct WorkerProcess
{
    pid_t pid;
    void (*run_ev_loop)(struct WorkerProcess *worker);
} WorkerProcess;

WorkerProcess *worker_process_init(MasterProcess *master);

#endif