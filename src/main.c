#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "conf.h"
#include "shm.h"
#include "hashtable.h"

int main(int argc, char **argv)
{
    ServerConfig *config = server_conf_init(CONF_FILE);
    SharedMemory *shm = shared_memory_init();

    int *a = (int *)shm->alloc(sizeof(int));
    *a = 5;

    HashTable *ht = create_hash_table();
    const char* key = "filename";

    ht->set(ht, key, &config->filename);
    const char *value = *((const char **)ht->get(ht, key));

    printf("%s\n", value);
    printf("Length: %ld, Capacity: %ld\n", ht->length, ht->capacity);
    printf("%d\n", *a);

    exit(0);
}