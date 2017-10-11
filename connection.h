#ifndef CONN_HEAD
#define CONN_HEAD

#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <string.h>     /* String handling */
#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <arpa/inet.h>
extern int sockfd;

void setup_server_conns(int);

#endif