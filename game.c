#include <stdlib.h>     /* General Utilities */
#include <string.h>     /* String handling */
#include <stdio.h>      /* Input/Output */
#include <unistd.h>     /* Symbolic Constants */
#include "game.h"


void* play_game(void *args) {
    int *connfd;
    connfd = (int *) args;
    char *buffer;
    memset(buffer, '0', sizeof(buffer));
    buffer = "This is server saying Hi!\n";
    write(*connfd, buffer, strlen(buffer));
}
