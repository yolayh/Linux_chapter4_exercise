#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>


#define SIZE 4096

int main(){

    int pd1[2], pd2[2], pid;
    char c_buf[SIZE], p_buf[SIZE];
    int rrn, rn;

    if (pipe(pd1) == -1){
        perror("pipe_one error");
        exit(EXIT_FAILURE);
    }

    if (pipe(pd2) == -1){
        perror("pipe_two error");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    switch(pid){
    
        case -1:
            perror("fork error");
            exit(EXIT_FAILURE);
        
        // child
        case 0:
            // close
            if (close(pd1[1]) == -1){
                perror("close pd1[1] error");
                exit(EXIT_FAILURE);
            }   
            if (close(pd2[0]) == -1){
                perror("close pd2[0] error");
                exit(EXIT_FAILURE);
            }

            while(1){
                // read from pipe
                rrn = read(pd1[0], c_buf, SIZE);
                if (rrn == -1){
                    perror("read error");
                    exit(EXIT_FAILURE);
                }
                else{
                    // write to pd2
                    for (int i = 0; i < rrn; i++){
                        c_buf[i] = toupper(c_buf[i]);
                    }
                    if (write(pd2[1], c_buf, rrn) == -1){
				        perror("writing to pipe error");
				        exit(EXIT_FAILURE);
		            }  

                }
            }


        // parent
        default:
            // close
            if (close(pd1[0]) == -1){
                perror("parent close pd1[0] error");
                exit(EXIT_FAILURE);
            } 
            if (close(pd2[1]) == -1){
                perror("parent close pd2[1] error");
                exit(EXIT_FAILURE);
            } 

            // first, parent read from standard input
            while(1){   
                rn = read(STDIN_FILENO, p_buf, SIZE);
                if (rn == -1){
                    perror("parent reading standard input error");
                    exit(EXIT_FAILURE);
                }
                else{
                    if (write(pd1[1], p_buf, rn) == -1){
				        perror("parent writing to pipe error");
				        exit(EXIT_FAILURE);
		            }  
        	    }

                rn = read(pd2[0], p_buf, SIZE);
                if (rn == -1){
                    perror("parent read error");
                    exit(EXIT_FAILURE);
                }
                else{
			        if (write(STDOUT_FILENO, p_buf, rn) == -1){
				        perror("parent writing standard output error");
				        exit(EXIT_FAILURE);
		            }
        	    }    
            }
    }

    exit(EXIT_SUCCESS);

}

