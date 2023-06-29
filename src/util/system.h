#ifndef SYSTEM_H
#define SYSTEM_H

typedef struct System
{
    int cpu_num;
    int page_size;
    int (*bind_cpu)(int cpuid);
} System;

System *system_init();

#endif