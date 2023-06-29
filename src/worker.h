#ifndef WORKER_H
#define WORKER_H

#include "master.h"
#include "server.h"

typedef struct WorkerProcess
{
    pid_t pid;
    int cpuid;
    size_t max_fds;
    void (*run_ev_loop)(struct WorkerProcess *worker, TCPServer *server);
} WorkerProcess;

WorkerProcess *worker_process_init(MasterProcess *master, ServerConfig *conf);

#endif