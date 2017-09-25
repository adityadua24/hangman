// Author: Aditya Dua
// Date: 11-Sept, 2017
// Unit: CAB403 Systems Programming
// Assignment: Process Management and Distributed Computing
/*---------------------------------------------------------------*/
/* ----- IMPORT HEADER FILES-------*/
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <assert.h>
#include <termios.h>

/*--- GLOBAL VARIABLES -------------*/
int sockfd = 0;

/*
Function to handle SIGINT. Ensures clean exit of server.
param - sigNum: Signal number corresponding to singal
*/
void signal_handler(int sigNum){
    printf("\nSafely exiting code ....\n");
    close(sockfd); // Close the socket. 0 if successful, -1 if error
    exit(sigNum);
}
/*
Source of this function code is the hyper link below.
https://stackoverflow.com/questions/1786532/c-command-line-password-input

Buffer size and string termination is modified to suits this program.
*/

void getPassword(char password[])
{
    static struct termios oldt, newt;
    int i = 0;
    int c;

    /*saving the old settings of STDIN_FILENO and copy settings for resetting*/
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;

    /*setting the approriate bit in the termios struct*/
    newt.c_lflag &= ~(ECHO);          

    /*setting the new bits*/
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);

    /*reading the password from the console*/
    while ((c = getchar())!= '\n' && c != EOF && i < 8){
        password[i++] = c;
    }
    password[i] = '\n';
    password[++i] = '\0';

    /*resetting our old STDIN_FILENO*/ 
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);

}

int send_segment(int *sockfd, char *msg, int msg_len){
    char terminate = '#';
    int n = 0;
    for(int i=0; i < msg_len; i++){
        if ((n = send(*sockfd, (msg+i), 1, 0)) <= 0){
            printf("Sending failed ....\n");
            return -1;
        }
    }
    if ((n = send(*sockfd, &terminate, 1, 0)) <=0 ){
        printf("Segment termination failed ....\n");
        return -1;
    }
    return 1;
}

char * read_segment(int *sockfd){
    int n, i = 0;
    char *ch;
    ch = (char *)malloc(sizeof(char));
    memset(ch, '0', 1);
    char *buffer;
    buffer = (char *)malloc(sizeof(char) * 256);
    memset(buffer, '0', 256);
    while(1){
        n = recv(*sockfd, ch, 1, 0);
        if (n == 0){
            return NULL;
        }
        else if(n == -1){
            printf("Receiving failed ....\n");
            exit(-1);
        }
        if(*ch != '#'){
            *(buffer+i) = *ch;
        }
        else{
            *(buffer+i) = '\0';
            break;
        }
        i++;
    }
    return buffer;
}
int main(int argc, char const *argv[]) {

    if (argc < 3) {
        printf("Invalid number of arguemnts passed.\n"
        "server_ip and port number expected.\n");
        exit(-1);
    }
    int port = atoi(argv[2]);
    struct sockaddr_in serv_addr;
    inet_aton(argv[1], &serv_addr.sin_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    char *buffer;
    char *ch;
    ch = (char *) malloc(sizeof(char));
    memset(ch, '0', 1);
    buffer = (char *)malloc(sizeof(char) * 500);
    memset(buffer, '0', 500);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        // AF_INET: IPv4, SOCK_STREAM: 2-way comm, 0: Default protocol
        printf("Failed to create a socket.\nExiting now....\n");
        exit(-1);
    }

    if((connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) < 0){
        printf("Failed to connect to server.\nExiting now....\n");
        exit(-1);
    }

    int n = 0;

    while(1){
        char *msg = read_segment(&sockfd);
        if(msg == NULL){
            printf("Receiving failed ....\n");
            printf("Exiting now ....\n");
            exit(-1);
        }
        int flag = 0;
        if (strlen(msg) == 1){
            flag = atoi(msg);
        }
        switch (flag){
            case 1:{
                    char *username = (char *)malloc(sizeof(char) * 10);
                    memset(username, '0', 10);
                    // char username[10];
                    // gets(username);
                    fgets(username, 10, stdin);
                    if ((send_segment(&sockfd, username, strlen(username))) == -1){
                        printf("Disconnecting ....\n");
                        exit(-1);
                    }
                    }
                    break;
            case 2:{
                    char *pswrd = (char *)malloc(sizeof(char) * 10);
                    memset(pswrd, '0', 10);
                    // fgets(pswrd, 10, stdin);
                    getPassword(pswrd);
                    printf("\n");
                    if((send_segment(&sockfd, pswrd, strlen(pswrd))) == -1){
                        printf("Disconnecting ....\n");
                        exit(-1);
                    }
                }
                break;
            default:
                printf("%s", msg);
                fflush(stdout);
                break;
        }
    }
}
