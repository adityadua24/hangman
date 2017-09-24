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

extern int sockfd;

void* play_game(void *);
int authenticate(int *);
char* read_segment(int *);
int send_segment(int *connfd, char *msg, int msg_len);
#endif