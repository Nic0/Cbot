/***********************************************************
*
*       CBot v0.2.2       (28/04/10)
*
*   by Nic0 <nicolas.caen at gmail.com>
*   Si vous redistribuez ce code, merci de ne pas effacer
*   ces données, et de rajouter les vôtres. (GPL)
*
************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#include "main.h"
#include "config.h"
#include "socket.h"
#include "utils.h"

#define MAXBUFF 512
#define NBR_ELEMENT 6

#define ERROR fprintf (stderr, \
        "%s:%d Error (%d) : %s\n", \
        __FILE__, __LINE__, \
        errno, strerror(errno))

int main (void)
{   
    struct sockaddr_in sockname;
    struct hostent *host_address = NULL;
    config_t config;
    data_t data; 
    initData(&data);

    if ((getConfiguration (&config)) == 0) {
        if ((initSocket (&data.sock, &sockname, host_address, &config)) == 0)
            if (initSocketConnect (&data.sock, &config) == 0)
                freeStruct (&config);
    } else {
        close (data.sock);
        return EXIT_FAILURE;
    }

    pthread_t pidrecv;
    
    if (pthread_create (&pidrecv, NULL, recvSocket, &data) != 0) {
        ERROR;
        close (data.sock);
        return EXIT_FAILURE;
    }
    
    pthread_join (pidrecv, NULL);
    
    return EXIT_SUCCESS;
}
