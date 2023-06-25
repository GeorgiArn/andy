#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "worker.h"

static void run_ev_loop(WorkerProcess *worker) {
    // TODO: accept and handle server connections
    printf("worker %d started...\n", worker->pid);
    fflush(stdout);

    sleep(2);

    printf("worker %d finished...\n", worker->pid);
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
    worker->run_ev_loop = run_ev_loop;

    return worker;
}