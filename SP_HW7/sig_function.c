#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

/* 
    implement sigset(), sighold(), sigrelse(), sigignore(), sigpause() -> by POSIX signal API

    typedef void(* sighandler_t)(int);
    sighandler_t sigset(int sig，sighandler_t disp);
    int sighold(int sig);
    int sigrelse(int sig);
    int sigignore(int sig);
    int sigpause(int sig);
*/

typedef void (*sighandler_t)(int);

// void sighandler_func(int signum){
//     printf("  catch signal %d\n", signum);
// }

sighandler_t my_sigset(int sig, sighandler_t handler){
    
    struct sigaction newdisp, olddisp;
    
    newdisp.sa_handler = handler;
    sigemptyset(&newdisp.sa_mask);
    newdisp.sa_flags = 0;

    if (sigaction(sig, &newdisp, &olddisp) == -1){
        perror("sigaction error");
        exit(EXIT_FAILURE);
    }
    else{
        return olddisp.sa_handler;
    }
}


int my_sighold(int sig){

    sigset_t mask; 
    int return_value;

    sigemptyset(&mask);
    sigaddset(&mask, sig);
    return_value = sigprocmask(SIG_BLOCK, &mask, NULL);
    if(return_value == -1){
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    return return_value;
}


int my_sigrelse(int sig){

    sigset_t mask;
    int return_value;

    sigemptyset(&mask);
    sigaddset(&mask, sig);
    return_value = sigprocmask(SIG_UNBLOCK, &mask, NULL);
    if(return_value == -1){
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    return return_value;
}

int my_sigignore(int sig){

    int return_value;
    struct sigaction act;

    sigemptyset(&act.sa_mask);
    act.sa_handler = SIG_IGN;

    return_value = sigaction(sig, &act, NULL);
    if (return_value == -1){
        perror("sigaction - SIG_IGN");
        exit(EXIT_FAILURE);
    }
    else{
        return return_value;
    }
}

int my_sigpause(int sig){

    int return_value;
    sigset_t current_mask, empty;
    sigemptyset(&empty);
    if (sigprocmask(SIG_BLOCK, NULL, &current_mask) == -1){
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }
    sigdelset(&current_mask, sig);
    return_value = sigsuspend(&current_mask);
    if (return_value == -1 && errno != EINTR){
        perror("sigsuspend");
        exit(EXIT_FAILURE);
    }

    return return_value;
}


int main(){

//     my_sigset(SIGINT, sighandler_func);
//     my_sighold(SIGINT);
//     my_sigrelse(SIGINT);
//     my_sigignore(SIGINT);
//     my_sigpause(SIGINT);
//     while(1);
    exit(EXIT_SUCCESS);
}