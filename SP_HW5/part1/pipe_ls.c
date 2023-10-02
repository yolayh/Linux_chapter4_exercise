#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define SIZE 4096

int main(){
    
    int pd[2], pid;
    char buf[SIZE];

    if (pipe(pd) == -1){
        perror("pipe error");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    switch(pid){

        case -1:
            perror("fork error");
            exit(EXIT_FAILURE);
        
        // child
        case 0:
            close(pd[0]);

            //let "ls" write to pipe
            if ((dup2(pd[1], STDOUT_FILENO)) == -1){
                perror("dup2 error");
                exit(EXIT_FAILURE);
            }
            close(pd[1]);

            if ((execlp("ls", "ls", "-l", NULL)) == -1){
                perror("execlp error");
                exit(EXIT_FAILURE);
            }
            break;

        // parent
        default:
            break;
    }   

    close(pd[1]);

    // read from pipe
    if ((read(pd[0], buf, SIZE)) == -1){
        perror("read error");
        exit(EXIT_FAILURE);
    }
    printf("%s", buf);
    close(pd[0]);

    exit(EXIT_SUCCESS);
}