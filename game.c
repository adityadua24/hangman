#include <stdlib.h>     /* General Utilities */
#include <string.h>     /* String handling */
#include <stdio.h>      /* Input/Output */
#include <unistd.h>     /* Symbolic Constants */
#include "game.h"

void* play_game(void *args) {
    int *exit_status = (int *)malloc(sizeof(int));
    request *game_session = (request *)args;
    session_info *this_session = (session_info *)malloc(sizeof(session_info));
    int *connfd = &(game_session->connfd);
    printf("game session connfd: %d\n", *connfd);
    char buffer[] = "\n================================================\n\nWelcome to the Online Hangman Gaming System\n\n================================================\n\n";
    // send(*connfd, buffer, strlen(buffer), 0);
    // int x = authenticate(connfd);
    int n = 0; 
    if ((n = send_segment(connfd, buffer, strlen(buffer))) == 0){
        return NULL;
    }
    int login = authenticate(connfd, this_session);
    if (login == 0){
        char *fail = "\nYou entered an incorrect username or password.\nDisconnecting ....\n";
        if ((send_segment(connfd, fail, strlen(fail))) == 0){
            *exit_status = 0;
            return (void *)exit_status;
        }
        close(*connfd);
    }else if(login == 1){
        int opt = options(connfd);
        while(1){
            if(opt == 1){
                printf("option 1 selected\n");
                int status = start_playing(connfd, this_session);
                opt = options(connfd);
            }
            else if(opt == 2){
                printf("option 2 selected\n");
                show_leaderboard(connfd);
                opt = options(connfd);
            }
            else if(opt == 3){
                printf("option 3 selected\n");
                quit(connfd);
            }
            else{
                char *invalid = "Invalid selection ....\nDisconnecting ....\n";
                int sent = send_segment(connfd, invalid, strlen(invalid));
                if (sent == 0){
                    *exit_status = 0;
                    return (void *)exit_status;
                }
                opt = options(connfd);
            }
        }
    }
    printf("Sleeping......\n");
    sleep(10);
}

int authenticate(int *connfd, session_info *this_session){
    char *output = "You are required to logon with your registered Username and Password\nPlease enter your username--> ";
    int sent = send_segment(connfd, output, strlen(output));
    char *flag;
    flag = "1";
    sent = send_segment(connfd, flag, strlen(flag));
    char *name = read_segment(connfd);
    if (name == NULL){
        printf("Failed to receive username ....\n");
        return 0;
    }
    *(name + (strlen(name)-1)) = '\0'; // Removes trailing new line character
    output = "Please enter your password--> ";
    sent = send_segment(connfd, output, strlen(output));
    flag = "2";
    sent = send_segment(connfd, flag, strlen(flag));
    char *pswrd = read_segment(connfd);
    if (pswrd  == NULL){
        printf("Failed to receive password ....\n");
        return 0;
    }
    *(pswrd + (strlen(pswrd)-1)) = '\0'; // Removes trailing new line character
    update_session_info(this_session, name, pswrd);
    // printf("password is: %s\n", pswrd);
    // printf("password lenght is: %d\n", strlen(pswrd));
    for(int i=0; i < user_count; i++){
        if (strcmp((*(user_names+i)), name) == 0){
            if(strcmp((*(passwords+i)), pswrd) == 0){
                char *success = "\n=====================================\n    Authentication sucessfull   \n=====================================\n\n";
                send_segment(connfd, success, strlen(success));
                return 1;
            }
        } 
    }
    return 0;
}

int options(int *connfd){
    char *select = "Please enter a selection\n<1> Play Hangman\n<2> Show Leaderboard\n<3> Quit\n\nSelect option--> ";
    send_segment(connfd, select, strlen(select));
    char *flag = "3";
    send_segment(connfd, flag, strlen(flag));
    char *selection = read_segment(connfd);
    printf("Selection is : %s\n", selection);
    if (selection == NULL){
        return 0;
    }
    else if(*selection == '\n'){
        return 0;
    }
    else if (*selection == '1'){
        return 1;
    }
    else if(*selection == '2'){
        return 2;
    }
    else if(*selection == '3'){
        return 3;
    }
    else{
        return 0;
    }
}
int start_playing(int *connfd, session_info *this_session){
    int select = (int) random_at_most(comb_count-1);
    char *pair;
    pair = (char *)malloc(sizeof(char) * strlen(*(combinations+select)));
    strcpy(pair, *(combinations+select));
    int num_guess = min(strlen(pair)-1+10, 26); // strlen(pair)-1 to remove space between words 
    char *got_right = form_got_right(pair);
    char *guessed = (char *)malloc(sizeof(char) * num_guess);
    if ((game_ui(connfd, guessed, &num_guess, got_right, 0)) == 0){
        return 0;
    }
    while(num_guess > 0){
        char *guess = read_segment(connfd);
        if (guess == NULL){
            return 0;
        }
        num_guess--;
        match_guess_pair(guess, pair, got_right);
        guessed = strcat(guessed, guess);
        if((strchr(got_right, '_')) == NULL){
            if ((game_ui(connfd, guessed, &num_guess, got_right, 1)) == 0){
                return 0;
            }
            break;
        }
        else{
            if((game_ui(connfd, guessed, &num_guess, got_right, 0)) == 0){
                return 0;
            }
        }
    }
    if ((game_over(connfd, num_guess, this_session)) == 0){
        return 0;
    }
    return 1;
}

int quit(int *connfd){
    return 0;
}

int show_leaderboard(int *connfd){
    return 0;
}

/*
Code retreived from the following hyperlink. Return uniformly distributed random numbers between 0 and max.
https://stackoverflow.com/questions/2509679/how-to-generate-a-random-number-from-within-a-range
*/
long random_at_most(long max) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    /* using nano-seconds instead of seconds */
    srandom((time_t)ts.tv_nsec);
    unsigned long
      // max <= RAND_MAX < ULONG_MAX, so this is okay.
      num_bins = (unsigned long) max + 1,
      num_rand = (unsigned long) RAND_MAX + 1,
      bin_size = num_rand / num_bins,
      defect   = num_rand % num_bins;
  
    long x;
    do {
     x = random();
    }
    // This is carefully written not to overflow
    while (num_rand - defect <= (unsigned long)x);
  
    // Truncated division is intentional
    return x/bin_size;
  }

  int game_ui(int *connfd, char *guessed, int *num_guess, char *got_right, int last){
    char *ui;
    ui = (char *)malloc(sizeof(char)*256);
    char *head = "\n------------------------------------\n";
    ui = strcat(ui, head);
    char *line1= "\nGuessed letters: ";
    ui = strcat(ui, line1);
    ui = strcat(ui, guessed);
    char *line2=  "\n\nNumber of guesses left: ";
    ui = strcat(ui, line2);
    char line2_data[3];
    sprintf(line2_data, "%d", *num_guess);
    ui = strcat(ui, line2_data);
    char *line3 = "\n\nWord: ";
    ui = strcat(ui, line3);
    ui = strcat(ui, got_right);
    char *line4 = "\n\nEnter your guess - ";
    if(last != 1){
        ui = strcat(ui, line4);
        if ((send_segment(connfd, ui, strlen(ui))) == 0){
            return 0;
        }
        char *flag = "4";
        if ((send_segment(connfd, flag, strlen(flag))) == 0){
            return 0;
        }
    }
    else{
        if((send_segment(connfd, ui, strlen(ui))) == 0){
            return 0;
        }
    }
    return 1;
}

  int min(int x, int y){
      if (x >= y){
          return y;
      }
      else {
          return x;
      }
  }

  char* form_got_right(char *pair){
    char *got_right = (char *)malloc(sizeof(char)*2*strlen(pair)+1); // +1 for string terminator
    *(got_right + 2*strlen(pair)) = '\0';
    char *encodeChar = "_ ";
    char *encodeSpace = "  ";
    for(int i =0; i < strlen(pair); i++){
        if(*(pair+i) == ' '){
            got_right = strcat(got_right, encodeSpace);
        }
        else {
            got_right = strcat(got_right, encodeChar);
        }
    }
    return got_right;
  }

  void match_guess_pair(char *guess, char *pair, char *got_right){
      for(int i=0; i < strlen(pair); i++){
          if (*(pair+i) == *guess){
              *(got_right+i*2) = *guess;
          }
      }
  }

  int game_over(int *connfd, int status, session_info *this_session){
    char *msg = (char *)malloc(sizeof(char)*256);
    if (status > 0){
        char *seg = "\n\nGame Over!\n\nWell done ";
        msg = strcat(msg, seg);
        msg = strcat(msg, this_session->user);
        seg = "! You won this round of Hangman!\n";
        msg = strcat(msg, seg);
        if ((send_segment(connfd, msg, strlen(msg))) == 0){
            return 0;
        }
    }
    else{
        char *seg = "\nGame Over!\nBad luck ";
        msg = strcat(msg, seg);
        msg = strcat(msg, this_session->user);
        seg = "! You have run out of guesses. The Hangman got you!";
        msg = strcat(msg, seg);
        char *newline = "\n\n";
        msg = strcat(msg, newline);
        if((send_segment(connfd, msg, strlen(msg))) == 0){
            return 0;
        }
    }
    return 1;
  }

  void update_session_info(session_info *this_session, char *name, char *pswrd){
      this_session->user = (char *)malloc(strlen(name));
      this_session->password = (char *)malloc(strlen(pswrd));
      strcpy(this_session->user, name);
      strcpy(this_session->password, pswrd);
  }