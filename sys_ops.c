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
            play_game(game_session);
        }
        else{
            rc = pthread_cond_wait(&got_request, &request_mutex);
        }
        sleep(1);
    }
}

void add_request(request *new){
    int rc = pthread_mutex_lock(&request_mutex);
    if(num_requests == 0) {
        requests = new;
        printf("ALL GOOD CHECKPOINT: 1\n");
    }
    else if( num_requests > 0) {
        new->next = requests;
        requests = new;
        printf("ALL GOOD CHECKPOINT: 2\n");
    }
    num_requests++;
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
