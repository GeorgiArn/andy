#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "conf.h"

int main(int argc, char **argv)
{
    ServerConfig* config = server_conf_init(CONF_FILE);
    const char *port = config->get_entry("listen");

    printf("%s\n", config->filename);
    printf("%s\n", port);

    exit(0);
}