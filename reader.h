#ifndef READER_HEAD
#define READER_HEAD

extern int user_count;
extern int comb_count;

void count_users(void);
void count_combinations(void);
void read_authentication(char ***, char ***);      
void read_hangman(char ***);

#endif