/*****************************
*       IRCBOT
*           Nic0 (24/04/10)
******************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 6665
#define HOST "senseii.tonbnc.fr"

#define ERROR fprintf (stderr, \
        "%s:%d Error (%d) : %s\n", \
        __FILE__, __LINE__, \
        errno, strerror(errno))

int initSocket (int *sock, struct sockaddr_in *sockname, struct hostent *host_address);
int initSocketConnect (int *sock);

int main (void)
{   
    char buffer [2048] = {0};
    struct sockaddr_in sockname;
    struct hostent *host_address;
    int sock = 0;

    if ((initSocket (&sock, &sockname, host_address)) == 1)
        return EXIT_FAILURE;

    if (initSocketConnect (&sock) == 1)
    {
        close (sock);
        return EXIT_FAILURE;
    }

    while(1)
    {
        recv(sock, buffer, sizeof(buffer) - 1, 0);
        printf("%s", buffer);
        if (strstr(buffer, "ERROR :") != NULL)
        {
            close (sock);
            return EXIT_FAILURE;
        }
    }
    
    
    return EXIT_SUCCESS;
}

int initSocket (int *sock, struct sockaddr_in *sockname, struct hostent *host_address)
{
/*  Obtention de l'adresse de la machine distante à partir de l'host.
 */
    if ((host_address = gethostbyname (HOST)) == NULL)
    {
        ERROR;
        return 1;
    }

/*  Création d'une socket.
 */
    if ((*sock = socket (PF_INET, SOCK_STREAM, 0)) == -1)
    {
        ERROR;
        return 1;
    }

/*  Connexion de la socket au server distant
 */
    sockname->sin_family = host_address ->h_addrtype;
    sockname->sin_port = htons (PORT);
    memcpy ((char *) &((*sockname).sin_addr.s_addr), host_address->h_addr, host_address->h_length);

    if ((connect (*sock, (struct sockaddr *) sockname, sizeof (struct sockaddr_in))) == -1)
    {
        ERROR;
        return 1;
    }
    return 0;
}


int initSocketConnect (int *sock)
{

    char *nick = "NICK Hazardous\r\n";
    char *user = "USER Haz \"localhost\" \"irc_server\" :Nic0 s Bot\r\n";
    char *join = "JOIN #test\r\n"; 

    if ((send(*sock, nick, strlen(nick), 0)) == -1)
    {
        ERROR;
        return 1;
    }
    
    if ((send(*sock, user, strlen(user), 0)) == -1)
    {
        ERROR;
        return 1;
    }
    
    if ((send(*sock, join, strlen(join), 0)) == -1)
    {
        ERROR;
        return 1;
    }

    return 0;
}
