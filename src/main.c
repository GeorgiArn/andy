#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    pid_t pid = fork();
    if (pid > 0) {
        printf("Process id: %d\n", pid);
        exit(0);
    }

    return 0;
}