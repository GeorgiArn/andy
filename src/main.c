#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "conf.h"

int main(int argc, char **argv)
{
    loadConfig(CONF_FILE);

    const char *port = getConfValue("listen");
    printf("%s\n", port);

    exit(0);
}