#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <string.h>     /* String handling */
#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <arpa/inet.h>
#include "connection.h"

void setup_server_conns(int port){
    struct sockaddr_in serv_addr;
    // inet_aton("127.0.0.1", &serv_addr.sin_addr);  // Set IP address for socket
    serv_addr.sin_port = htons(port); // Converts port number to network byte order before assigning
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Set IP address for socket
    serv_addr.sin_family = AF_INET;
    // struct sockaddr_in cli_addr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        // AF_INET: IPv4, SOCK_STREAM: 2-way comm, 0: Default protocol
        printf("Failed to create a socket.\nExiting now....\n");
        exit(-1);
    }
    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        printf("Failed to bind socket.\nExiting now....\n");
        exit(-1);
    }
    if (listen(sockfd, 10) == -1){ // Convert unconnected socket to passive socket
        printf("Failed to listen at port %d\n", port);
        exit(-1);
    }
}