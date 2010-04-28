#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED


typedef struct Buffer
{
    char *buff;
    char *salon;
    struct Buffer *next;
} buffer_t;

typedef struct Data
{
    buffer_t *first;
    buffer_t *last;
    int count;
    int sock;
    pthread_mutex_t mutex;
} data_t;

#endif
