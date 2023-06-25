#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "master.h"
#include "worker.h"

static bool shall_spawn_worker = true;

static size_t get_workers_count(ServerConfig *config)
{
    // TODO: get workers count from config
    return 4;
}

static void spawn_workers(MasterProcess *master, WorkerProcess *workers[])
{
    for (size_t i = 0; i < master->workers_num; i++)
    {
        if (workers[i] == NULL)
            workers[i] = worker_process_init(master);

        if (workers[i]->pid == -1)
        {
            pid_t pid = fork();

            switch (pid)
            {
            case -1:
                printf("Failed to fork worker process. \n");
                exit(0);
            case 0:
                // Child process
                workers[i]->pid = getpid();
                workers[i]->run_ev_loop(workers[i]);
                exit(0);
                break;
            default:
                // Parent process
                workers[i]->pid = pid;
                break;
            }
        }
    }
}

static void run_ev_loop(MasterProcess *master)
{
    WorkerProcess* workers[master->workers_num];
    for (size_t i = 0; i < master->workers_num; i++)
        workers[i] = NULL;

    while (true)
    {
        if (shall_spawn_worker) {
            shall_spawn_worker = false;
            spawn_workers(master, workers);
        }

        sleep(1);
    }
}

MasterProcess *master_process_init(ServerConfig *config)
{
    MasterProcess *master = (MasterProcess *)malloc(sizeof(MasterProcess));
    if (master == NULL)
    {
        printf("Failed to allocate memory for master process. \n");
        exit(0);
    }

    master->pid = getpid();
    master->workers_num = get_workers_count(config);

    master->run_ev_loop = run_ev_loop;

    return master;
}