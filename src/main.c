#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "conf.h"

int main(int argc, char **argv)
{
    loadConfig(CONF_FILE);

    exit(0);
}