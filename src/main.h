#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED


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

#endif
