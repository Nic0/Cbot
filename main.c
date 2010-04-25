/***************************************
*       IRCBOT
*           Nic0 (24/04/10)
****************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAXBUFF 512

#define ERROR fprintf (stderr, \
        "%s:%d Error (%d) : %s\n", \
        __FILE__, __LINE__, \
        errno, strerror(errno))

struct Configuration
{
    char *host;
    int port;
    char *nick;
    char *ident;
    char *realname;
    char *salon;
};

int initSocket (int *sock, struct sockaddr_in *sockname, 
                struct hostent *host_address, struct Configuration *config);
int initSocketConnect (int *sock, struct Configuration *config);
int socketAction (int *sock, char *buffer);
int pong (int *sock, char *buffer);
int getConfiguration (struct Configuration *config);
int extractConfig (char *element);
char *concatString (char *str1, char *str2);



int main (void)
{   
    struct sockaddr_in sockname;
    struct hostent *host_address;
    struct Configuration config;
    int sock = 0;

    if ((getConfiguration (&config)) == 1)
        return EXIT_FAILURE;
    
    if ((initSocket (&sock, &sockname, host_address, &config)) == 1)
        return EXIT_FAILURE;

    if (initSocketConnect (&sock, &config) == 1) {
        close (sock);
        return EXIT_FAILURE;
    }

    while(1) {
        char buffer [2048] = {0};
        if ((recv(sock, buffer, sizeof(buffer) - 1, 0)) == -1) {
            ERROR;
            close (sock);
            return EXIT_FAILURE;
        }
        printf("%s", buffer);

        if ((socketAction (&sock, buffer)) == 1)
            ERROR;
    }
    
    return EXIT_SUCCESS;
}

int initSocket (int *sock, struct sockaddr_in *sockname, 
                struct hostent  *host_address, struct Configuration *config)
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


int initSocketConnect (int *sock, struct Configuration *config)
{

    char *user = "USER Haz \"localhost\" \"irc_server\" :Nic0 s Bot\r\n";
    char *join = "JOIN #test\r\n"; 

    if ((send(*sock, config->nick, strlen(config->nick), 0)) != -1)
        if ((send(*sock, user, strlen(user), 0)) != -1)
            if ((send(*sock, join, strlen(join), 0)) != -1)
                return 0;
    ERROR;
    return 1;
}

int pong (int *sock, char *buffer)
{
    buffer[1] = 'O';
    if ((send(*sock, buffer, strlen(buffer), 0)) == -1) {
        ERROR;
        return 1;
    }
    return 0;
}

int socketAction (int *sock, char *buffer)
{
    if (strstr(buffer, "PING :") != NULL && buffer[0] == 'P') {
        if ((pong(sock, buffer)) ==1) {
            close(*sock);
            return 1;
        }
    }

    if (strstr(buffer, "ERROR :Closing Link:") != NULL && buffer[0] == 'E') {
        close (*sock);
        return 1;
    }
	return 0;
}

int getConfiguration (struct Configuration *config)
{
    FILE *fichier;
    char filebuff[MAXBUFF] = {0};

    fichier = fopen ("bot.conf", "r");
    if (fichier != NULL) {
        while ((fgets (filebuff, MAXBUFF, fichier)) != NULL) {

            if (filebuff[0] == '#')
                continue;

            if ((strstr (filebuff, "host")) != NULL) {    
                if ((config->host = (strdup(strchr (filebuff, '"')))) != NULL) {
                    if (extractConfig(config->host) == 0)
                        continue;
                }
                else {
                    ERROR;
                    return 1;
                }
            }

            if ((strstr (filebuff, "port")) != NULL) {
                config->port = 0;
                char *tmp = NULL;
                tmp = strdup (strchr (filebuff, '"'));

                if (extractConfig(tmp) == 0)
                    if ((config->port = atoi(tmp)) != 0)
                        continue;
                ERROR;
                return 1;
            }

            if ((strstr (filebuff, "nick")) != NULL) {
                char *tmp = NULL;
                config->nick = strdup ("NICK ");
                if ((tmp = strdup (strchr (filebuff, '"'))) != NULL)
                    if (extractConfig (tmp) == 0)
                        if ((concatString (config->nick, tmp)) != NULL)
                            if ((concatString (config->nick, "\r\n")) != NULL)
                                continue;
                ERROR;
                return 1;
            }
                
                

        }
    }
    else {
        ERROR;
        return 1;
    }
    return 0;
}

int extractConfig (char *element)
{
    int i = 0;
    do {
        element[i] = element[i+1];
        i++;
    }while (element[i+1] != '"' && i <= 1000);

    if (i >= 1000) {
        fprintf(stderr, "An error has occured in the configuration file, a double-quote is missing\n");
        return 1;
    }
    element[i] = '\0';
    return 0;
}

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
