#include <stdlib.h>     /* General Utilities */
#include <string.h>     /* String handling */
#include <stdio.h>      /* Input/Output */
#include <unistd.h>     /* Symbolic Constants */
#include "game.h"

void* play_game(void *args) {
    request *game_session = (request *)args;
    int *connfd = &(game_session->connfd);
    printf("game session connfd: %d\n", *connfd);
    char buffer[] = "\n================================================\n\nWelcome to the Online Hangman Gaming System\n\n================================================\n";
    // send(*connfd, buffer, strlen(buffer), 0);
    // int x = authenticate(connfd);
    int n = 0; 
    if ((n = send_segment(connfd, buffer, strlen(buffer))) == -1){
        return NULL;
    }
    if (authenticate(connfd) == -1){
        char *fail = "Authentication failed. Invalid username or password.\nDisconnecting ....\n";
        send_segment(connfd, fail, strlen(fail));
        close(*connfd);
    }
    printf("Sleeping......\n");
    sleep(10);
}

int authenticate(int *connfd){
    char output[] = "You are required to logon with your registered Username and Password\nPlease enter your username--> ";
    int sent = send_segment(connfd, output, strlen(output));
    char *flag = "1";
    // sleep(1);
    sent = send_segment(connfd, flag, strlen(flag));
    // user = (char *)malloc(sizeof(char) * 10);
    // memset(user, '0', 10);
    // int rec = recv(*connfd, user, 10, 0);
    // *(user+rec) = '\0';
    // printf("%s\n", user);
    // char *flag = "1 ";
    // send(*connfd, flag, strlen(flag), 0);
    // int n;
    // char input[256];
    // if ((read(*connfd, input, sizeof(input)-1)) != -1){
    //     printf("Error reading from client ....\n");
    // }
    return 0;
}