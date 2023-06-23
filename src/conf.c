#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>

#include "conf.h"
#include "hashtable.h"

#define KEY_VALUE_LEN 32
#define BUFFER_LEN (KEY_VALUE_LEN * 2) + 3

typedef struct str_t
{
    unsigned char *p;
    size_t len;
} str_t;

static HashTable *config_map = NULL;

static int parse_entry(unsigned char *row, str_t *key, str_t *val)
{
    unsigned char *c = row;

    // skip whitespaces (if any)
    while ((isspace(*c) || !isprint(*c)) && *c != '\0')
        c++;

    // return if entry is a comment
    if (*c == '#' || *c == '\0')
        return -1;

    key->p = c;
    key->len = 0;

    // iterate key
    while (!isspace(*c) && isprint(*c) && *c != '\0' && *c != '=' && *c != '#')
    {
        key->len++;
        c++;
    }

    // continue to start of val
    while ((isspace(*c) || !isprint(*c) || *c == '=') && *c != '\0')
        c++;

    val->p = c;
    val->len = 0;

    // iterate val
    while (!isspace(*c) && isprint(*c) && *c != '\0' && *c != '=' && *c != '#')
    {
        val->len++;
        c++;
    }

    return 0;
}

static void load_config(const char *filename)
{
    FILE *fd = fopen(filename, "r");
    config_map = create_hash_table();

    if (fd == NULL)
    {
        printf("Configuration file can't be opened. \n");
        exit(0);
    }

    char row[BUFFER_LEN];
    while (fgets(row, BUFFER_LEN, fd))
    {
        str_t key, val;

        if (parse_entry(row, &key, &val) == -1)
            continue;

        if (key.len > KEY_VALUE_LEN || val.len > KEY_VALUE_LEN)
        {
            printf("Config entry is too long. Maximum allowed key and value width: %d\n%.*s = %.*s \n",
                   KEY_VALUE_LEN, (int)key.len, key.p, (int)val.len, val.p);

            fclose(fd);
            exit(0);
        }

        char map_key[KEY_VALUE_LEN];
        char *map_value = (char *)malloc(KEY_VALUE_LEN * sizeof(char));

        if (map_value == NULL)
        {
            printf("Failed to allocate config map entry value. \n");
            exit(0);
        }

        memcpy(map_key, key.p, key.len);
        memcpy(map_value, val.p, val.len);

        map_key[key.len] = '\0';
        map_value[val.len] = '\0';

        config_map->set(config_map, map_key, (void*) map_value);
    }

    fclose(fd);
}

static char *get_entry(const char *key)
{
    if (config_map == NULL)
    {
        printf("Config map is not initialized. \n");
        exit(0);
    }

    return config_map->get(config_map, key);
}

ServerConfig *server_conf_init(const char *filename)
{
    if (config_map != NULL)
    {
        printf("Server config is already initalized. \n");
        exit(0);
    }

    ServerConfig *config = (ServerConfig *)malloc(sizeof(ServerConfig));
    if (config == NULL)
    {
        printf("Failed to allocate memory for server config. \n");
        exit(0);
    }

    load_config(filename);
    config->filename = filename;
    config->get_entry = get_entry;

    return config;
}