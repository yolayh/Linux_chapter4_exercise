#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>


#define TESTSIG SIGUSR1

int main(int argc, char *argv[])
{
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        printf("%s num-sigs\n", argv[0]);
        exit(EXIT_FAILURE);

    int numSigs = atoi(argv[1]);

    siginfo_t si;
    sigset_t sig_input;

    // Block the signal before fork
    sigemptyset(&sig_input);
    sigaddset(&sig_input, TESTSIG);
    if (sigprocmask(SIG_BLOCK, &sig_input, NULL) == -1)
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    
    pid_t childPid = fork();
    switch (childPid) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE); 

    // child
    case 0: 
        for (int scnt = 0; scnt < numSigs; scnt++) {
            if (kill(getppid(), TESTSIG) == -1)
                perror("kill");
                exit(EXIT_FAILURE); 
            if (sigwaitinfo(&sig_input, &si)  == -1 && errno != EINTR)
                perror("sigwaitinfo");
                exit(EXIT_FAILURE); 
        }
        exit(EXIT_SUCCESS);

    // parent
    default:
        for (int scnt = 0; scnt < numSigs; scnt++) {
            if (sigwaitinfo(&sig_input, &si) == -1 && errno != EINTR)
                perror("sigwaitinfo");
                exit(EXIT_FAILURE); 
            if (kill(childPid, TESTSIG) == -1)
                perror("kill");
                exit(EXIT_FAILURE); 
        }
        exit(EXIT_SUCCESS);
    }
}