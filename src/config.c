#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "config.h"
#include "main.h"
#include "utils.h"

#define MAXBUFF 512
#define NBR_ELEMENT 6

#define ERROR fprintf (stderr, \
        "%s:%d Error (%d) : %s\n", \
        __FILE__, __LINE__, \
        errno, strerror(errno))


/*  Fonction permettant de récupérer les données lu dans le fichier de configuration
 *  Chaque éléments reconnu est stocké dans la structure Configuration, 
 *  Il lui est rajouté dans cette fonction le nécessaire pour être envoyer directement
 *  dans la socket (une fois sortie de cette fonction, tout est prêt à l'emploi.
 *  Si un # est rencontré en 1er caractère, on considère que c'est un commentaire
 *  et est alors exclu.
 */
int getConfiguration (config_t *config)
{
    FILE *fichier;
    char filebuff[MAXBUFF] = {0};
    int nbrElement = 0;

    fichier = fopen ("bot.conf", "r");
    if (fichier != NULL) {
        while ((fgets (filebuff, MAXBUFF, fichier)) != NULL) {

            if (filebuff[0] == '#')
                continue;

            if ((strncmp (filebuff, "host", 4)) == 0) {    
                if ((config->host = (strdup(strchr (filebuff, '"')))) != NULL)
                    if (extractConfig(config->host) == 0) {
                        nbrElement++;
                        continue;
                    }
                ERROR;
                return 1;
            }

            if ((strncmp (filebuff, "port", 4)) == 0) {
                config->port = 0;
                char *tmp = NULL;
                if ((tmp = strdup (strchr (filebuff, '"'))) != NULL)
                    if (extractConfig(tmp) == 0)
                        if ((config->port = atoi(tmp)) != 0) {
                            nbrElement++;
                            free(tmp);
                            continue;
                        }
                ERROR;
                return 1;
            }

            if ((strncmp (filebuff, "nick", 4)) == 0) {
                char *tmp = NULL;
                config->nick = strdup ("NICK ");
                if ((tmp = strdup (strchr (filebuff, '"'))) != NULL)
                    if (extractConfig (tmp) == 0)
                        if ((concatString (config->nick, tmp)) != NULL)
                            if ((concatString (config->nick, "\r\n")) != NULL) {
                                nbrElement++;
                                free(tmp);
                                continue;
                            }
                ERROR;
                return 1;
            }

            if ((strncmp (filebuff, "ident", 5)) == 0) {
                if ((config->ident = (strdup(strchr (filebuff, '"')))) != NULL)
                    if (extractConfig(config->ident) == 0) {
                        nbrElement++;
                        continue;
                    }
                ERROR;
                return 1;
            }
            
            if ((strncmp (filebuff, "realname", 8)) == 0) {
                if ((config->realname = (strdup(strchr (filebuff, '"')))) != NULL)
                    if (extractConfig(config->realname) == 0) {
                        nbrElement++;
                        continue;
                    }
                ERROR;
                return 1;
            }

            if ((strncmp (filebuff, "salon", 5)) == 0) {
                char *tmp = NULL;
                config->salon = strdup ("JOIN ");
                if ((tmp = strdup (strchr (filebuff, '"'))) != NULL)
                    if (extractConfig (tmp) == 0)
                        if ((concatString (config->salon, tmp)) != NULL)
                            if ((concatString (config->salon, "\r\n")) != NULL) {
                                nbrElement++;
                                free(tmp);
                                continue;
                            }
                ERROR;
                return 1;
            }
        }
        fclose (fichier);
    }
    else {
        ERROR;
        return 1;
    }
    if (nbrElement == NBR_ELEMENT) {
        if ((config->user = strdup("USER ")) != NULL)
            if ((concatString (config->user, config->ident)) != NULL)
                if ((concatString (config->user, " \"localhost\" \"irc_server\" :")) != NULL)
                    if ((concatString (config->user, config->realname)) != NULL)
                        if ((concatString (config->user, "\r\n")) != NULL)
                            return 0;
        ERROR;
        return 1;
    }
    else {
        fprintf (stderr, "The configuration file doesn't seem to contain enought or to much data\n");
        return 1;
    }
    return 0;
}
/*  Utilitaire: lorsque l'élément en question est lu dans le fichier de configuration
 *  il apparait sous la forme "element", cette fonction enlève donc les "" pour ne laisser
 *  que l'élément.
 */
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

/*  Une fois les informations envoyé, on libère la structure.
 */
void freeStruct (config_t *config)
{
    free(config->host);
    free(config->nick);
    free(config->ident);
    free(config->realname);
    free(config->user);
    free(config->salon);
}
