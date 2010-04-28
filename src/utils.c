#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "main.h"

/*  Utilitaire permettant de concaténer deux chaine de façon dynamique
 */
char *concatString (char *str1, char *str2)
{
    char *result;
    int len1, len2;
    len1 = strlen(str1);
    len2 = strlen(str2);
 
    if(NULL == (result = realloc (str1, (len1 + len2 +1) * sizeof *result)))
        return (NULL);
 
    memcpy (result + len1, str2, len2 + 1);
    return result;
}

void initData (data_t *data)
{
    data->first = NULL;
    data->last = NULL;
    data->count = 0;
    data->sock = 0;
    pthread_mutex_init (&data->mutex, NULL);
}
