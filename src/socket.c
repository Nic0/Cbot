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

/*  Fonction permettant de créer la socket et de la connecter au serveur IRC.
 */
int initSocket (int *sock, struct sockaddr_in *sockname, 
                struct hostent  *host_address, config_t *config)
{
/*  Obtention de l'adresse de la machine distante à partir de l'host.
 */
    if ((host_address = gethostbyname (config->host)) == NULL) {
        ERROR;
        return 1;
    }

/*  Création d'une socket.
 */
    if ((*sock = socket (PF_INET, SOCK_STREAM, 0)) == -1) {
        ERROR;
        return 1;
    }

/*  Connexion de la socket au server distant
 */
    sockname->sin_family = host_address ->h_addrtype;
    sockname->sin_port = htons (config->port);
    memcpy ((char *) &(sockname->sin_addr.s_addr), host_address->h_addr, host_address->h_length);

    if ((connect (*sock, (struct sockaddr *) sockname, sizeof (struct sockaddr_in))) == -1) {
        ERROR;
        return 1;
    }
    return 0;
}

/*  Fonction permettant d'envoyer les paramètres de bases pour rejoindre un salon, avec
 *  le choix du pseudo et identifiant
 */
int initSocketConnect (int *sock, config_t *config)
{
    if ((send(*sock, config->nick, strlen(config->nick), 0)) != -1)
        if ((send(*sock, config->user, strlen(config->user), 0)) != -1)
            if ((send(*sock, config->salon, strlen(config->salon), 0)) != -1)
                return 0;
    ERROR;
    return 1;
}

/*  Fonction envoyant la réponse au ping
 */
int pong (int *sock, char *buffer)
{
    buffer[1] = 'O';
    if ((send(*sock, buffer, strlen(buffer), 0)) == -1) {
        ERROR;
        return 1;
    }
    return 0;
}

/*  Fonction traitant le buffer pour savoir quel action on doit lui associer.
 *  Ici, seul la réponse au Ping est traité et le cas de fermeture de la socket
 *  par le serveur distant.
 *  Pour rajouter des fonctionnalité au bot, 
 *  c'est ici qu'il est le plus simple de le faire.
 */
int socketAction (int *sock, char *buffer)
{
    if (strncmp(buffer, "PING :", 6) == 0) {
        if ((pong(sock, buffer)) ==1) {
            close(*sock);
            return 1;
        }
    }

    if (strncmp(buffer, "ERROR :Closing Link:", 20) == 0) {
        close (*sock);
        return 1;
    }
	return 0;
}

void *recvSocket (void *arg)
{
    data_t *data = arg;

    while(1) {
        buffer_t buffer;
        buffer.buff = malloc (sizeof buffer);
        buffer.next = NULL;
        char bufftmp[2048] = {0};

        if ((recv (data->sock, bufftmp, sizeof (bufftmp) -1, 0)) != -1) {
            buffer.buff = strdup (bufftmp);
            printf("%s", buffer.buff);

            if ((socketAction (&data->sock, buffer.buff)) == 1)
                ERROR;
        } else {
            ERROR;
            close (data->sock);
        }
    }
}

