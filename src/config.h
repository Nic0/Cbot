#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include "main.h"

typedef struct
{
    char *host;
    int port;
    char *nick;
    char *ident;
    char *realname;
    char *user;
    char *salon;
} config_t;

int getConfiguration (config_t *config);
int extractConfig (char *element);
void freeStruct ( config_t *config);

#endif
