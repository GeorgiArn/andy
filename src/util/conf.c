#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>

#include "conf.h"

#define KEY_VALUE_LEN 128
#define BUFFER_LEN (KEY_VALUE_LEN * 2) + 3

typedef struct ConfigEntry
{
    char key[KEY_VALUE_LEN];
    char val[KEY_VALUE_LEN];
    struct ConfigEntry *next;
} ConfigEntry;

typedef struct EntriesList
{
    ConfigEntry *head;
    size_t length;
} EntriesList;

typedef struct str_t
{
    unsigned char *p;
    size_t len;
} str_t;

static EntriesList *list = NULL;

static EntriesList *entries_list_init()
{
    list = (EntriesList *)malloc(sizeof(EntriesList));
    if (list == NULL)
    {
        printf("Failed to allocated memory for config entries. \n");
        exit(0);
    }

    list->head = NULL;
    list->length = 0;

    return list;
}

static int add_entry(ConfigEntry *entry)
{
    ConfigEntry *next = list->head;
    list->head = entry;
    entry->next = next;

    list->length++;

    return 0;
}

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
    EntriesList *list = entries_list_init();
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

        ConfigEntry *entry = (ConfigEntry *)malloc(sizeof(ConfigEntry));
        if (entry == NULL)
        {
            printf("Failed to allocate config entry. \n");
            exit(0);
        }

        if (key.len > KEY_VALUE_LEN || val.len > KEY_VALUE_LEN)
        {
            printf("Config entry is too long. Maximum allowed key and value width: %d\n%.*s = %.*s \n",
                   KEY_VALUE_LEN, (int)key.len, key.p, (int)val.len, val.p);

            fclose(fd);
            exit(0);
        }

        memcpy(entry->key, key.p, key.len);
        memcpy(entry->val, val.p, val.len);

        add_entry(entry);
    }

    fclose(fd);
}

static const char *get_entry(const char *key)
{
    if (list == NULL || list->head == NULL)
    {
        printf("Config list is not initialized or it's empty. \n");
        exit(0);
    }

    ConfigEntry *entry = list->head;

    while (entry != NULL)
    {
        if (strcmp(entry->key, key) == 0)
            return entry->val;

        entry = entry->next;
    }

    printf("Config entry for '%s' can't be found. \n", key);
    exit(0);
}

ServerConfig *server_conf_init(const char *filename)
{
    if (list != NULL)
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