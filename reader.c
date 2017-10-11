#include "reader.h"

void count_users(void) {
    FILE *fp;
    char line[500];
    fp = fopen("Authentication.txt", "r");
    if (fp == NULL) {
        printf("Couldn't open file\n");
        exit(-1);
    }
    while(fgets(line, sizeof(line), fp) != NULL) user_count++;
    --user_count;
    fclose(fp);
}

void count_combinations(void) {
    FILE *fp;
    char line[500];
    fp = fopen("hangman_text.txt", "r");
    if (fp == NULL) {
        printf("Couldn't open file\n");
        exit(-1);
    }
    while(fgets(line, sizeof(line), fp) != NULL) comb_count++;
    fclose(fp);
}

void read_authentication(char ***user_names, char ***passwords){
    FILE *fp;
    char line[500];
    int name_len = 10;
    int pass_len = 10;
    fp = fopen("Authentication.txt", "r");
    if (fp == NULL) {
        printf("Couldn't open file\n");
        exit(-1);
    }
    fgets(line, sizeof(line), fp); // Skip first line

    for(int i=0; i < user_count; i++){
        fgets(line, sizeof(line), fp);
        char *split = strtok(line, " \t");
        strcpy(*(*(user_names)+i), split);
        split = strtok(NULL, " \t");  // Second split in 8 char in length. DONT KNOW WHY. SHOULD BE 6.
        strcpy(*(*(passwords)+i), split);
    }
    fclose(fp);
    // # pragma omp parallel
    for(int i=0; i < user_count; i++){
        *(*((*passwords)+i) + 6) = '\0';  // Terminate password after 6th character
    }
}

void read_hangman(char ***combinations){
    FILE *fp;
    char line[500];
    
    fp = fopen("hangman_text.txt", "r");
    if (fp == NULL) {
        printf("Couldn't open file\n");
        exit(-1);
    }

    for(int i=0; i < comb_count; i++){
        fgets(line, sizeof(line), fp);
        if (strlen(line) > 2){
            for(int j=0; j < strlen(line); j++){
                if(line[j] == ','){
                    *((*((*combinations)+i))+j) = ' ';
                }
                else if(line[j] == '\n'){
                    // Do Nothing
                }
                else {
                    *((*((*combinations)+i))+j) = line[j];
                }
            }
        }
    }
    fclose(fp);
}