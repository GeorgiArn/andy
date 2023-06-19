#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>

#include "conf.h"

#define KEY_VALUE_LEN 128

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

static EntriesList *initializeEntriesList()
{
    EntriesList *list = (EntriesList *)malloc(sizeof(EntriesList));
    if (list == NULL)
    {
        printf("Failed to allocated memory for config entries. \n");
        exit(0);
    }

    list->head = NULL;
    list->length = 0;

    return list;
}

static int addEntry(EntriesList *list, ConfigEntry *entry)
{
    if (list->head == NULL)
    {
        list->head = entry;
        list->length = 1;
        return 0;
    }

    ConfigEntry *prev = list->head;
    while (prev->next != NULL)
        prev = prev->next = NULL;

    prev->next = entry;
    list->length++;

    return 0;
}

static int parseEntry(unsigned char *row, str_t *key, str_t *val)
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

void loadConfig(const char *filename)
{
    FILE *fd = fopen(filename, "r");
    EntriesList *list = initializeEntriesList();
    if (fd == NULL)
    {
        printf("Configuration file can't be opened. \n");
        exit(0);
    }

    char row[256];
    while (fgets(row, 256, fd))
    {
        str_t key, val;

        if (parseEntry(row, &key, &val) == -1)
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

        addEntry(list, entry);
    }

    fclose(fd);
}