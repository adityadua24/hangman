#ifndef OPS_HEAD
#define OPS_HEAD

extern int user_count;
extern int comb_count;

void malloc_users_passwords(char ***, char ***);
void malloc_combinations(char ***);
void free_memory_users_passwords(char ***, char ***);
void free_memory_combinations(char ***);

#endif