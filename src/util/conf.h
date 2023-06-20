#ifndef CONF_H
#define CONF_H

typedef struct ServerConfig
{
    const char* filename;
    const char *(*get_entry)(const char *key);
} ServerConfig;

ServerConfig *server_conf_init(const char *filename);

#endif