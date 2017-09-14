// Author: Aditya Dua
// Date: 11-Sept, 2017
// Unit: CAB403 Systems Programming
// Assignment: Process Management and Distributed Computing
/*---------------------------------------------------------------*/

/* ----- IMPORT HEADER FILES-------*/
#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <string.h>     /* String handling */
#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>    /* POSIX Threads */
#include <errno.h>      /* Errors */
#include <omp.h>        /* OMP API */

/*----------- DEFINE ---------------*/
#define CONN_LIMIT 10
#define NAME_PASS_LEN 10
#define PHRASE_LEN 20
/*----------------------------------*/


/*--- GLOBAL VARIABLES -------------*/
int sockfd = 0;
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
void* hangman_instance(void *args) {
    int *connfd;
    connfd = (int *) args;
    char *buffer;
    memset(buffer, '0', sizeof(buffer));
    buffer = "This is server saying Hi!\n";
    write(*connfd, buffer, strlen(buffer));
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
    FILE *fp;
    char line[1000];
    char **user_names;
    char **passwords;
    char **objects;
    int num_objects;
    int name_len, pass_len = NAME_PASS_LEN;
    int phrase_len = PHRASE_LEN;
    int user_count = -1;
    pthread_t p_threads[CONN_LIMIT];
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len;
    cli_len = sizeof(cli_addr);

    /*---------------------------------------------*/

    if (argc < 2) {
        port = 12345;
    }else if (argc == 2) {
        port = atoi(argv[1]); // Port number to bind socket on
    }

    /*---------------------------------------------*/
    /*-----AUTHENTICATION.TXT OPERATIONS-----------*/
    /*---------------------------------------------*/
    fp = fopen("Authentication.txt", "r");
    if (fp == NULL) {
        printf("Couldn't open file\n");
        return 1;
    }
    while(fgets(line, sizeof(line), fp) != NULL) user_count++;
    printf("Total users: %d\n", user_count);
    rewind(fp);

    user_names = (char **)malloc(sizeof(char *) * user_count);
    passwords = (char **)malloc(sizeof(char *) * user_count);
    # pragma omp parallel
    for(int i=0; i < user_count; i++){
        *(user_names+i) = (char *)malloc(sizeof(char) * name_len);
        *(passwords+i) = (char *)malloc(sizeof(char) * pass_len);
    }
    fgets(line, sizeof(line), fp); // Skip first line

    for(int i=0; i < user_count; i++){
        fgets(line, sizeof(line), fp);
        char *split = strtok(line, " \t");
        strcpy(*(user_names+i), split);
        split = strtok(NULL, " \t");  // Second split in 8 char in length. DONT KNOW WHY. SHOULD BE 6.
        strcpy(*(passwords+i), split);
    }
    fclose(fp);
    # pragma omp parallel
    for(int i=0; i < user_count; i++){
        *(*(passwords+i) + 6) = '\0';  // Terminate password after 6th character
    }

    // for(int i=0; i < user_count; i++) {
    //     printf("%s:", *(user_names+i));
    //     printf("%s\n", *(passwords+i));
    //     printf("Length of password is: %d\n", strlen(*(passwords+i)));
    // }
    /*---------------------------------------------*/

    /*---------------------------------------------*/
    /*------------HANGMAN.TXT OPERATIONS-----------*/
    /*---------------------------------------------*/
    fp = fopen("hangman_text.txt", "r");
    if (fp == NULL) {
        printf("Couldn't open file\n");
        return 1;
    }
    while(fgets(line, sizeof(line), fp) != NULL) num_objects++;
    printf("Total combos: %d\n", num_objects);
    rewind(fp);

    objects = (char **)malloc(sizeof(char *) * num_objects);
    for(int i=0; i < num_objects; i++){
        *(objects+i) = (char *)malloc(sizeof(char) * phrase_len);
    }

    for(int i=0; i < num_objects; i++){
        fgets(line, sizeof(line), fp);
        if (strlen(line) > 2){
            for(int j=0; j < strlen(line); j++){
                if(line[j] != ','){
                    *((*(objects+i))+j) = line[j];
                }
                else {
                    *((*(objects+i))+j) = ' ';
                }
            }
        }
    }
    fclose(fp);
    printf("Object category and type sanity check....\n");
    for(int i =0; i < num_objects; i++){
        printf("%s", *(objects+i));  
    }
    printf("\n");
    /*---------------------------------------------*/


    /*------------------------- SOCKET CREATION CODE--------------------------------------------*/
    signal(SIGINT, signal_handler); // Registers signal handling function to kernel

    inet_aton("127.0.0.1", &serv_addr.sin_addr);  // Set IP address for socket
    serv_addr.sin_port = htons(port); // Converts port number to network byte order before assigning
    // serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Set IP address for socket
    serv_addr.sin_family = AF_INET;
    // struct sockaddr_in cli_addr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        // AF_INET: IPv4, SOCK_STREAM: 2-way comm, 0: Default protocol
        printf("Failed to create a socket.\nExiting now....\n");
        exit(-1);
    }
    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        printf("Failed to bind socket.\nExiting now....\n");
        exit(-1);
    }
    listen(sockfd, CONN_LIMIT);  // Convert unconnected socket to passive socket
    /*------------------------- SOCKET CREATION CODE--------------------------------------------*/

    /*------------------------- LOOP --------------------------------------------*/
    while(1) {
        printf("Listening at port %d\n", port);
        connfd[0] = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_len);
        /*
            accept() is blocking. Waits for the connection before running.
            Dequeues the next connection on the queue for socket.
        */
        if (pthread_create(&p_threads[0], NULL, &hangman_instance, (void *) &connfd[0]) != 0) {
            printf("Error creating thread");
            exit(-1);
        }
        printf("Thread created.\n");
        sleep(1);
    }
    /*------------------------- LOOP --------------------------------------------*/
    return 0;
}
