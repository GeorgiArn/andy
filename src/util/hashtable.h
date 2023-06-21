#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "unistd.h"

typedef struct HTEntry
{
    const char *key;
    void *value;
} HTEntry;

typedef struct HashTable
{
    HTEntry* entries;
    size_t length;
    size_t capacity;

    void (*set)(struct HashTable* ht, const char *key, void *value);
    void *(*get)(struct HashTable* ht, const char *key);
    void (*destroy)(struct HashTable* ht);
} HashTable;

HashTable *create_hash_table();

#endif