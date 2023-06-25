#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "worker.h"

static void run_ev_loop(WorkerProcess *worker, TCPServer *server) {
    // TODO: accept and handle server connections
    printf("Worker handling connections on %s:%d \n", server->host, server->port);
    printf("worker (pid: %d, cpuid: %d) started. \n", worker->pid, worker->cpuid);
    fflush(stdout);

    sleep(10);

    printf("worker (pid: %d, cpuid: %d) finished. \n", worker->pid, worker->cpuid);
    fflush(stdout);
}

WorkerProcess *worker_process_init(MasterProcess *master)
{
    WorkerProcess *worker = (WorkerProcess *)malloc(sizeof(WorkerProcess));
    if (worker == NULL)
    {
        printf("Failed to allocate memory for worker process. \n");
        exit(0);
    }

    worker->pid = -1;
    worker->cpuid = -1;
    worker->run_ev_loop = run_ev_loop;

    return worker;
}