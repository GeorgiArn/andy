#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/sysinfo.h>

#include "system.h"

static int bind_cpu(int cpuid)
{
    cpu_set_t cpuset;

    CPU_ZERO(&cpuset);
    CPU_SET(cpuid, &cpuset);

    if (sched_setaffinity(0, sizeof(cpuset), &cpuset) == -1)
        return -1;

    return 0;
}

System *system_init()
{
    System *sys = (System*) malloc(sizeof(System));
    if (sys == NULL)
    {
        printf("Failed to allocate memmory for system interface. \n");
        exit(0);
    }

    sys->cpu_num = get_nprocs_conf();
    sys->page_size = sysconf(_SC_NPROCESSORS_CONF);
    sys->bind_cpu = bind_cpu;

    return sys;
}