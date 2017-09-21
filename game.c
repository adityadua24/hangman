#include <stdlib.h>     /* General Utilities */
#include <string.h>     /* String handling */
#include <stdio.h>      /* Input/Output */
#include <unistd.h>     /* Symbolic Constants */
#include "game.h"

void* play_game(void *args) {
    request *game_session = (request *)args;
    int connfd = game_session->connfd;
    printf("game session connfd: %d\n", game_session->connfd);
    char *buffer = "Login and Play hangman! !\n";
    write(connfd, buffer, strlen(buffer));
    sleep(20);
}
