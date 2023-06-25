#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "master.h"
#include "worker.h"

static System* g_system = NULL;

static bool g_shall_spawn_worker = true;

static size_t get_workers_count(ServerConfig *config)
{
    const char* workers_count_str = config->get_entry("workers_count");
    
    if (workers_count_str == NULL)
    {
        printf("Workers count must be provided in %s. \n", config->filename);
        exit(0);
    }

    if (strcmp(workers_count_str, "default") == 0)
        return g_system->cpu_num;
    
    int workers_count = atoi(workers_count_str);
    if (workers_count <= 0)
    {
        printf("Invalid workers count in %s. \n", config->filename);
        exit(0);
    }

    return workers_count;
}

static void spawn_workers(MasterProcess *master, WorkerProcess *workers[])
{
    for (size_t i = 0; i < master->workers_count; i++)
    {
        if (workers[i] == NULL)
        {
            workers[i] = worker_process_init(master);
            workers[i]->cpuid = i % g_system->cpu_num;
        }

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
                if (g_system->bind_cpu(workers[i]->cpuid) == -1)
                {
                    printf("Failed to bind cpu id of worker process. \n");
                    exit(0);
                }

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
    WorkerProcess* workers[master->workers_count];
    for (size_t i = 0; i < master->workers_count; i++)
        workers[i] = NULL;

    while (true)
    {
        if (g_shall_spawn_worker) {
            g_shall_spawn_worker = false;
            spawn_workers(master, workers);
        }

        sleep(1);
    }
}

MasterProcess *master_process_init(System *system, ServerConfig *config)
{
    MasterProcess *master = (MasterProcess *)malloc(sizeof(MasterProcess));
    if (master == NULL)
    {
        printf("Failed to allocate memory for master process. \n");
        exit(0);
    }

    g_system = system;

    master->pid = getpid();
    master->workers_count = get_workers_count(config);

    master->run_ev_loop = run_ev_loop;

    return master;
}