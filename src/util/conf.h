#ifndef CONF_H
#define CONF_H

typedef struct ServerConfig
{
    const char* filename;
    const char *(*getEntry)(const char *key);
} ServerConfig;

ServerConfig *initializeServerConfig(const char *filename);

#endif