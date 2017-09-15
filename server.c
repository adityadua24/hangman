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
/*----------------------------------*/

/*---------------------------------------------*/
/*  Function to handle SIGINT. Ensures clean exit of server.
    param - sigNum: Signal number corresponding to singal   */
void signal_handler(int sigNum){
    printf("\nSafely exiting code ....\n");
    close(sockfd); // Close the socket. 0 if successful, -1 if error
    /*
        FREE memory operations here
    */
    exit(sigNum);
}
/*---------------------------------------------*/
// void* hangman_instance(void *args) {
//     int *connfd;
//     connfd = (int *) args;
//     char *buffer;
//     memset(buffer, '0', sizeof(buffer));
//     buffer = "This is server saying Hi!\n";
//     write(*connfd, buffer, strlen(buffer));
// }
/*---------------------------------------------*/
/*  param: argc: Number of arguments passed to main
    param: argv: Array of string arguments passed to main  */
int main(int argc, char *argv[]) {

    /*---------------------------------------------*/
    /*------ VARIABLES in main function stack -----*/
    /*---------------------------------------------*/
    int port = 0;
    int connfd[CONN_LIMIT];
    char **user_names;
    char **passwords;
    char **combinations;
   
    pthread_t p_threads[CONN_LIMIT];
    // struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    socklen_t cli_len;
    cli_len = sizeof(cli_addr);

    /*---------------------------------------------*/

    if (argc < 2) {
        port = 12345;
    }else if (argc == 2) {
        port = atoi(argv[1]); // Port number to bind socket on
    }

    count_users();
    printf("Total users: %d\n", user_count);
    read_authentication(&user_names, &passwords);
    count_combinations();
    read_hangman(&combinations);

    // for(int i=0; i < 10; i++) {
    //     printf("%s:", *(user_names+i));
    //     printf("%s. ", *(passwords+i));
    //     printf("Length of password is: %d\n", strlen(*(passwords+i)));
    // }
    // printf("Object category and type sanity check....\n");
    // for(int i =0; i < comb_count; i++){
    //     printf("%s", *(combinations+i));  
    // }
    // printf("\n");
    signal(SIGINT, signal_handler); // Registers signal handling function to kernel
    setup_server_conns(port);

    /*------------------------- LOOP --------------------------------------------*/
    while(1) {
        printf("Listening at port %d\n", port);
        connfd[0] = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_len);
        /*
            accept() is blocking. Waits for the connection before running.
            Dequeues the next connection on the queue for socket.
        */
        if (pthread_create(&p_threads[0], NULL, &play_game, (void *) &connfd[0]) != 0) {
            printf("Error creating thread");
            exit(-1);
        }
        printf("Thread created.\n");
        printf("connection id is: %d\n", connfd[0]);
        sleep(1);
    }
    /*------------------------- LOOP --------------------------------------------*/
    return 0;
}
