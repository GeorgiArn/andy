#ifndef CONF_H
#define CONF_H

typedef struct
{
    char *(*getEntry)(const char *key);
} ServerConfig;

void loadConfig(const char *filename);
const char *getConfValue(const char *key);

#endif