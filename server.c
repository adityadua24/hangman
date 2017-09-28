// Author: Aditya Dua
// Date: 11-Sept, 2017
// Unit: CAB403 Systems Programming
// Assignment: Process Management and Distributed Computing
/*---------------------------------------------------------------*/

#include "server.h"


/*----------- DEFINE ---------------*/
#define CONN_LIMIT 10
/*----------------------------------*/


/*--- GLOBAL VARIABLES -------------*/
int sockfd = 0;
int user_count;
int comb_count;
char **user_names;
char **passwords;
char **combinations;
lb *leaderboard;
request *requests;
int num_requests = 0;
int num_lb_entries = 0;
pthread_mutex_t request_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
pthread_cond_t  got_request   = PTHREAD_COND_INITIALIZER;
/*----------------------------------*/

/*---------------------------------------------*/
/*  Function to handle SIGINT. Ensures clean exit of server.
    param - sigNum: Signal number corresponding to singal   */
void signal_handler(int sigNum){
    printf("\n---------------------------\n");
    printf("Safely exiting code ....\n");
    printf("Closing socket ....\n");
    close(sockfd); // Close the socket. 0 if successful, -1 if error
    printf("Freeing memory ....\n");
    free_memory_users_passwords(&user_names, &passwords);
    free_memory_combinations(&combinations);
    printf("Destroying thread pool ....\n");
    printf("---------------------------\n");
    exit(sigNum);
}
/*---------------------------------------------*/
void sanity_check(int i) {
    switch(i) {
        case 1:
            printf("Total users: %d\n", user_count);
            for(int j=0; j < user_count; j++) {
                printf("%s:", *(user_names+j));
                printf("%s\n", *(passwords+j));
            }
            break;
        case 2:
            printf("Total combinations: %d\n", comb_count);
            for(int j=0; j < comb_count; j++) {
                printf("%s\n", *(combinations+j));
            }
            break;
    }
}
/*---------------------------------------------*/
/*  param: argc: Number of arguments passed to main
    param: argv: Array of string arguments passed to main  */
int main(int argc, char *argv[]) {
    
    /*---------------------------------------------*/
    /*------ VARIABLES in main function stack -----*/
    /*---------------------------------------------*/
    int port = 0;
    int connfd[CONN_LIMIT];
    // LeaderBoard lb;
    
    int thread_id[CONN_LIMIT];
    pthread_t t_pool[CONN_LIMIT];
    struct sockaddr_in cli_addr;
    socklen_t cli_len;
    cli_len = sizeof(cli_addr);

    /*---------------------------------------------*/

    if (argc < 2) {
        port = 12345;
    }else if (argc == 2) {
        port = atoi(argv[1]); // Port number to bind socket on
    }

    signal(SIGINT, signal_handler); // Registers signal handling function to kernel
    count_users();
    count_combinations();
    malloc_users_passwords(&user_names, &passwords);
    malloc_combinations(&combinations);
    malloc_leaderboard(&leaderboard);
    read_authentication(&user_names, &passwords);
    read_hangman(&combinations);
    setup_threadpool(t_pool, thread_id, CONN_LIMIT);
    // sanity_check(1);
    // sanity_check(2);
    initialise_leaderboard(&user_names);
    setup_server_conns(port);
    
    printf("Listening at port %d\n", port);
    while(1) {
        request* new = (request *)malloc(sizeof(request));
        new->next = NULL;
        new->connfd = accept(sockfd, (struct sockaddr *) &(new->cli_addr), &(new->cli_len));
        if (new->connfd == -1){
            printf("Failed to accept connection.\n");
            printf("Client address: %s\n", inet_ntoa(new->cli_addr.sin_addr));
        }
        else{
            printf("Connection accepted.\n");
            printf("Client address: %\n", inet_ntoa(new->cli_addr.sin_addr));
        }
        add_request(new);
        sleep(1);
    }
    return 0;
}
