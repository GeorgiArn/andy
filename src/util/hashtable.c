#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "hashtable.h"

#define INITAL_CAPACITY 16
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

static uint64_t hash_key(const char *key)
{
    uint64_t hash = FNV_OFFSET;
    for (const char *p = key; *p; p++)
    {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

static void ht_set_entry(HTEntry *entries, size_t capacity,
                         const char *key, void *value, size_t *plength)
{
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(capacity - 1));

    while (entries[index].key != NULL)
    {
        if (strcmp(key, entries[index].key) == 0)
        {
            entries[index].value = value;
            return;
        }

        index++;
        if (index >= capacity)
            index = 0;
    }

    if (plength != NULL)
    {
        key = strdup(key);
        if (key == NULL)
        {
            printf("Failed to allocate hash table key. \n");
            exit(0);
        }
        (*plength)++;
    }
    entries[index].key = (char *)key;
    entries[index].value = value;
}

static int ht_expand(HashTable *table)
{
    size_t new_capacity = table->capacity * 2;
    if (new_capacity < table->capacity)
        return 0; // overflow (capacity would be too big)

    HTEntry *new_entries = calloc(new_capacity, sizeof(HTEntry));
    if (new_entries == NULL)
        return 0;

    for (size_t i = 0; i < table->capacity; i++)
    {
        HTEntry entry = table->entries[i];
        if (entry.key != NULL)
            ht_set_entry(new_entries, new_capacity, entry.key, entry.value, NULL);
    }

    free(table->entries);
    table->entries = new_entries;
    table->capacity = new_capacity;

    return 1;
}

static void set(HashTable *ht, const char *key, void *value)
{
    if (value == NULL)
    {
        printf("Invalid hashtable entry value. \n");
        exit(0);
    }

    if (ht->length >= ht->capacity / 2)
    {
        if (!ht_expand(ht)) {
            printf("Hash table capacity exceeded. \n");
            exit(0);
        }
    }

    ht_set_entry(ht->entries, ht->capacity, key, value, &ht->length);
}

static void *get(HashTable *ht, const char *key)
{
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(ht->capacity - 1));

    while (ht->entries[index].key != NULL)
    {
        if (strcmp(key, ht->entries[index].key) == 0)
            return ht->entries[index].value;

        index++;
        if (index >= ht->capacity)
            index = 0;
    }

    return NULL;
}

static void destroy(HashTable* ht)
{
    for (size_t i = 0; i < ht->capacity; i++)
        free((void *)ht->entries[i].key);

    free(ht->entries);
    free(ht);
}

HashTable *create_hash_table()
{
    HashTable *ht = (HashTable *)malloc(sizeof(HashTable));
    if (ht == NULL)
    {
        printf("Failed to allocate memory for hash table. \n");
        exit(0);
    }

    ht->get = get;
    ht->set = set;
    ht->destroy = destroy;
    ht->length = 0;
    ht->capacity = INITAL_CAPACITY;
    ht->entries = (HTEntry *)calloc(ht->capacity, sizeof(HTEntry));
    if (ht->entries == NULL)
    {
        printf("Failed to allocate memory for hash table. \n");
        free(ht);
        exit(0);
    }

    return ht;
}