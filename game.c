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
    int login = authenticate(connfd);
    if (login == 0){
        char *fail = "\nYou entered an incorrect username or password.\nDisconnecting ....\n";
        send_segment(connfd, fail, strlen(fail));
        close(*connfd);
    }else if(login == 1){
        ;
    }
    printf("Sleeping......\n");
    sleep(10);
}

int authenticate(int *connfd){
    char *output = "You are required to logon with your registered Username and Password\nPlease enter your username--> ";
    int sent = send_segment(connfd, output, strlen(output));
    char *flag;
    flag = "1";
    sent = send_segment(connfd, flag, strlen(flag));
    char *name = read_segment(connfd);
    if (name == NULL){
        printf("Failed to receive username ....\n");
        return 0;
    }
    *(name + (strlen(name)-1)) = '\0'; // Removes trailing new line character
    output = "Please enter your password--> ";
    sent = send_segment(connfd, output, strlen(output));
    flag = "2";
    sent = send_segment(connfd, flag, strlen(flag));
    char *pswrd = read_segment(connfd);
    if (pswrd  == NULL){
        printf("Failed to receive password ....\n");
        return 0;
    }
    *(pswrd + (strlen(pswrd)-1)) = '\0'; // Removes trailing new line character
    // printf("password is: %s\n", pswrd);
    // printf("password lenght is: %d\n", strlen(pswrd));
    for(int i=0; i < user_count; i++){
        if (strcmp((*(user_names+i)), name) == 0){
            if(strcmp((*(passwords+i)), pswrd) == 0){
                return 1;
            }
        } 
    }

    return 0;
}