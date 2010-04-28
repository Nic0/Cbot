#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED

int initSocket (int *sock, struct sockaddr_in *sockname, 
                struct hostent *host_address, config_t *config);
int initSocketConnect (int *sock, config_t *config);
int socketAction (int *sock, char *buffer);
int pong (int *sock, char *buffer);
void *recvSocket (void *arg);

#endif
