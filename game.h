#ifndef GAME_HEAD
#define GAME_HEAD

#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

typedef struct game_request {
    int connfd;
    struct sockaddr_in cli_addr;
    socklen_t cli_len;
    struct game_request *next;
}request;

typedef struct game_session{
    char *user;
    char *password;
}session_info;

extern int sockfd;
extern int user_count;
extern int comb_count;
extern char **user_names;
extern char **passwords;
extern char **combinations;

void* play_game(void *);
int authenticate(int *, session_info *);
int options(int*);
char* read_segment(int *);
int send_segment(int *connfd, char *msg, int msg_len);
int start_playing(int *, session_info *);
int show_leaderboard(int *);
int quit(int *);
long random_at_most(long);
int game_ui(int *, char *, int *, char *, int);
int min(int, int);
char* form_got_right(char *);
void match_guess_pair(char *, char *, char *);
int game_over(int *, int, session_info *);
void update_session_info(session_info *, char *, char *);
#endif