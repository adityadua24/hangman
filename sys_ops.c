#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <omp.h>
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