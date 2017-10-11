#ifndef OPS_HEAD
#define OPS_HEAD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "game.h"

extern int user_count;
extern int comb_count;
extern int num_requests;
extern request *requests;
extern pthread_cond_t got_request;

void malloc_users_passwords(char ***, char ***);
void malloc_combinations(char ***);
void malloc_leaderboard(lb **);
void free_memory_users_passwords(char ***, char ***);
void free_memory_combinations(char ***);
void setup_threadpool(pthread_t *, int *, int);
void destroy_threadpool(pthread_t *, int *, int);
void* game_requests_loop(void *);
void add_request(request *);
request* get_request();
void initialise_leaderboard(char ***);
void close_connections();
void clear_pending_requests();
#endif