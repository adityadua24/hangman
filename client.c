// Author: Aditya Dua
// Date: 11-Sept, 2017
// Unit: CAB403 Systems Programming
// Assignment: Process Management and Distributed Computing
/*---------------------------------------------------------------*/
/* ----- IMPORT HEADER FILES-------*/
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <assert.h>

/*--- GLOBAL VARIABLES -------------*/
int sockfd = 0;

/*
Function to handle SIGINT. Ensures clean exit of server.
param - sigNum: Signal number corresponding to singal
*/
void signal_handler(int sigNum){
    printf("\nSafely exiting code ....\n");
    close(sockfd); // Close the socket. 0 if successful, -1 if error
    exit(sigNum);
}

int main(int argc, char const *argv[]) {

    if (argc < 3) {
        printf("Invalid number of arguemnts passed.\n"
        "server_ip and port number expected.\n");
        exit(-1);
    }
    int port = atoi(argv[2]);
    struct sockaddr_in serv_addr;
    inet_aton(argv[1], &serv_addr.sin_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    char buffer[256];
    memset(buffer, '0', sizeof(buffer));

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        // AF_INET: IPv4, SOCK_STREAM: 2-way comm, 0: Default protocol
        printf("Failed to create a socket.\nExiting now....\n");
        exit(-1);
    }

    if((connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) < 0){
        printf("Failed to connect to server.\nExiting now....\n");
        exit(-1);
    }

    int n = 0;
    while((n = read(sockfd, buffer, sizeof(buffer) -1)) > 0) {
        buffer[n] = 0;
        if(fputs(buffer, stdout) == EOF)
        {
            printf("\n Error : Fputs error\n");
        }
    }



    return 0;
}
