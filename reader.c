#include "reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

void read_authentication(char **user_names, char **passwords){
    FILE *fp;
    char line[500];
    int user_count = -1;
    int name_len = 10;
    int pass_len = 10;
    fp = fopen("Authentication.txt", "r");
    if (fp == NULL) {
        printf("Couldn't open file\n");
        exit(-1);
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
}