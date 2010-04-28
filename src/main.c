/***********************************************************
*
*       CBot v0.2       (26/04/10)
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
    int sock = 0;

    if ((getConfiguration (&config)) == 1)
        return EXIT_FAILURE;
    
    if ((initSocket (&sock, &sockname, host_address, &config)) == 1)
        return EXIT_FAILURE;

    if (initSocketConnect (&sock, &config) == 1) {
        close (sock);
        return EXIT_FAILURE;
    }

    freeStruct (&config);
    
    while(1) {
        
        buffer_t buffer;
        buffer.buff = malloc(sizeof buffer);
        buffer.next = NULL;
        char bufftmp[2048] = {0};
        
        if ((recv(sock, bufftmp, sizeof(bufftmp) - 1, 0)) != -1) {
            buffer.buff = strdup (bufftmp);
            printf("%s", buffer.buff);

            if ((socketAction (&sock, buffer.buff)) == 1)
                ERROR;
        } else {
            ERROR;
            close(sock);
            return EXIT_FAILURE;
        }
    }
    
    return EXIT_SUCCESS;
}
