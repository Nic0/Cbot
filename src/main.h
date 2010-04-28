#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

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

typedef struct Buffer
{
    char *buff;
    char *salon;
    struct Buffer *next;
} buffer_t;

typedef struct File
{
    buffer_t first;
    buffer_t last;
    int count;
    pthread_mutex_t mutex;
} file_t;



int initSocket (int *sock, struct sockaddr_in *sockname, 
                struct hostent *host_address, config_t *config);
int initSocketConnect (int *sock, config_t *config);
int socketAction (int *sock, char *buffer);
int pong (int *sock, char *buffer);
char *concatString (char *str1, char *str2);
void freeStruct ( config_t *config);

int initFile (file_t *file);
int enfileBuffer (buffer_t *buffer, file_t *file);
void *fichierLog (void *arg);

#endif
