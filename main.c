#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 6665
#define HOST "senseii.tonbnc.fr"


int initSocketConnect (int sock);


int main (void)
{
    
    char buffer [2048] = {0};
    struct sockaddr_in sockname;
    struct hostent *host_address;
    int sock = 0;
    int *psock;
    psock = &sock;

/*  Obtention de l'adresse de la machine distante à partir de l'host.
 */
    if (NULL == (host_address = gethostbyname (HOST)))
    {
        fprintf (stderr, "Impossible d'indentifier la machine\n");
        return (-1);
    }

/*  Création d'une socket.
 */
    if (-1 == (sock = socket (PF_INET, SOCK_STREAM, 0)))
    {
        fprintf (stderr, "Impossible de créer une socket\n");
        return EXIT_FAILURE;
    }

/*  Connexion de la socket au server distant
 */
    sockname.sin_family = host_address ->h_addrtype;
    sockname.sin_port = htons (PORT);
    memcpy ((char *) &(sockname.sin_addr.s_addr), host_address->h_addr, host_address->h_length);

    if (-1 == (connect (sock, (struct sockaddr *) &sockname, sizeof (struct sockaddr_in))))
    {
        fprintf (stderr, "Impossible de connecter la socket au serveur\n");
        return (-1);
    }
    
    initSocketConnect (*psock);

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

int initSocketConnect (int sock)
{

    char *nick = "NICK Hazardous\r\n";
    char *user = "USER Haz \"localhost\" \"irc_server\" :Nic0 s Bot\r\n";

    if (send(sock, nick, strlen(nick), 0) == 0);
    {
        fprintf (stderr, "Impossible d'envoyer les identifiants pour la connexion\n");
        return EXIT_FAILURE;
    }
    
    if (send(sock, user, strlen(user), 0) == 0);
    {
        fprintf (stderr, "Impossible d'envoyer les identifiants pour la connexion\n");
        return EXIT_FAILURE;
    }
    return 0;
}
