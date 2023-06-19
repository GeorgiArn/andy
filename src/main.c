#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "conf.h"

int main(int argc, char **argv)
{
    ServerConfig* config = initializeServerConfig(CONF_FILE);
    const char *port = config->getEntry("listen");

    printf("%s\n", config->filename);
    printf("%s\n", port);

    exit(0);
}