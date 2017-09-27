#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <omp.h>
#include <pthread.h>
#include <unistd.h>
#include "sys_ops.h"


void malloc_users_passwords(char ***user_names, char ***passwords){
    int name_len = 10;
    int pass_len = 10;
    *(user_names) = (char **)malloc(sizeof(char *) * user_count);
    *(passwords) = (char **)malloc(sizeof(char *) * user_count);
    // # pragma omp parallel
    for(int i=0; i < user_count; i++){
        *(*(user_names)+i) = (char *)malloc(sizeof(char) * name_len);
        *(*(passwords)+i) = (char *)malloc(sizeof(char) * pass_len);
    }
}

void malloc_combinations(char ***combinations){
    int phrase_len = 20;
    *combinations = (char **)malloc(sizeof(char *) * comb_count);
    for(int i=0; i < comb_count; i++){
        *(*(combinations)+i) = (char *)malloc(sizeof(char) * phrase_len);
    }
}

void free_memory_users_passwords(char ***user_names, char ***passwords){
    for(int i=0; i < user_count; i++){
        free(*(*(user_names)+i));
        free(*(*(passwords)+i));
    }
    free(*(user_names));
    free(*(passwords));
}

void free_memory_combinations(char ***combinations){
    for(int i=0; i < comb_count; i++){
        free(*(*(combinations)+i));
    }
    free(*(combinations));
}

void setup_threadpool(pthread_t *t_pool, int *thread_id, int numThreads){
    for(int i=0; i < numThreads; i++){
        *(thread_id+i) = i;
        if(pthread_create(&t_pool[i], NULL, game_requests_loop, (thread_id+i)) !=0){
            printf("Error in creating thread number %d\n", i);
            exit(-1);
        }
    }
}

void* game_requests_loop(void *args){
    int thread_id = *(int *)args;
    int rc = pthread_mutex_lock(&request_mutex);
    while(1) {
        if(num_requests > 0) {
            request *game_session = get_request();
            if(game_session != NULL){
                rc = pthread_mutex_unlock(&request_mutex);
                int *status = (int *)play_game(game_session);
                if(*status == 0){
                    printf("Something went wrong with a client session.\n");
                }
                free(game_session);
                rc = pthread_mutex_lock(&request_mutex);
            }  
        }
        else{
            rc = pthread_cond_wait(&got_request, &request_mutex);
        }
    }
}

void add_request(request *new){
    int rc = pthread_mutex_lock(&request_mutex);
    if(num_requests == 0) {
        requests = new;
    }
    else if( num_requests > 0) {
        new->next = requests;
        requests = new;
    }
    num_requests++;
    printf("Request queued ....\n");
    rc = pthread_mutex_unlock(&request_mutex);
    rc = pthread_cond_signal(&got_request);
}

request* get_request(){
    request *pop;
    int rc = pthread_mutex_lock(&request_mutex);
    if(num_requests > 0){
        pop = requests;
        if(requests->next != NULL){
            requests = requests->next;
        }
        num_requests--;
    }
    else{
        pop = NULL;
    }
    rc = pthread_mutex_unlock(&request_mutex);
    return pop;
}

char * read_segment(int *connfd){
    int n, i = 0;
    char *ch;
    ch = (char *)malloc(sizeof(char));
    memset(ch, '0', 1);
    char *buffer;
    buffer = (char *)malloc(sizeof(char) * 256);
    memset(buffer, '0', 256);
    while(1){
        n = recv(*connfd, ch, 1, 0);
        if (n == 0){
            return NULL;
        }
        else if(n == -1){
            printf("Receiving failed ....\n");
            return NULL;
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

int send_segment(int *connfd, char *msg, int msg_len){
    char terminate = '#';
    int n = 0;
    for(int i=0; i < msg_len; i++){
        if ((n = send(*connfd, (msg+i), 1, 0)) <= 0){
            printf("Sending failed ....\n");
            return 0;
        }
    }
    if ((n = send(*connfd, &terminate, 1, 0)) <=0 ){
        printf("Segment termination failed ....\n");
        return 0;
    }
    return 1;
}
