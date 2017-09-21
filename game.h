#ifndef GAME_HEAD
#define GAME_HEAD

#include <sys/socket.h>
#include <arpa/inet.h>

typedef struct game_request {
    int connfd;
    struct sockaddr_in cli_addr;
    socklen_t cli_len;
    struct game_request *next;
}request;

void* play_game(void *);
#endif