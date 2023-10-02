/* 
 * pipe_command.c  :  deal with pipes
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "shell.h"

#define STD_OUTPUT 1
#define STD_INPUT  0
#define SIZE 4096

void pipe_and_exec(char **myArgv) {
  	int pipe_argv_index = pipe_present(myArgv);
  	int pipefds[2];
	char **left_argv;
	char **right_argv;
	char buf[SIZE];

  	switch (pipe_argv_index) {

    	case -1:	/* Pipe at beginning or at end of argv;  See pipe_present(). */
      		fputs ("Missing command next to pipe in commandline.\n", stderr);
      		errno = EINVAL;	/* Note this is NOT shell exit. */
      		break;

    	case 0:	/* No pipe found in argv array or at end of argv array.
			See pipe_present().  Exec with whole given argv array. */
			execvp(myArgv[0], myArgv);
      		break;

    	default:	/* Pipe in the middle of argv array.  See pipe_present(). */

      		/* Split arg vector into two where the pipe symbol was found.
       		 * Terminate first half of vector.
			 *
       		 * Fill in code. */
			left_argv = (char **)malloc((pipe_argv_index+1) * sizeof(char*));
			for (int i = 0; i < pipe_argv_index; i++){
				left_argv[i] = malloc((strlen(myArgv[i])+1) * sizeof(char));
				strcpy(left_argv[i], myArgv[i]);
			}
			left_argv[pipe_argv_index] = NULL;

			int index = pipe_argv_index+1;
			int i = 0;
			right_argv = (char **)malloc(1 * sizeof(char*));
			while(myArgv[index] != NULL){
				right_argv = (char **)realloc(right_argv, (i+2) * sizeof(char*));
				right_argv[i] = malloc((strlen(myArgv[index])+1) * sizeof(char));
				right_argv[i] = myArgv[index];
				i++;
				index++;
			}
			right_argv[i] = NULL;

      		/* Create a pipe to bridge the left and right halves of the vector. 
			 *
			 * Fill in code. */

			if (pipe(pipefds) == -1){
        		perror("pipe error");
        		exit(EXIT_FAILURE);
    		}		

      		/* Create a new process for the right side of the pipe.
       		 * (The left side is the running "parent".)
       		 *
			 * Fill in code to replace the underline. */
      		switch(fork()) {

        		case -1 :
	  				break;

        		/* Talking parent.  Remember this is a child forked from shell. */
        		default :

	  				/* - Redirect output of "parent" through the pipe.
	  				 * - Don't need read side of pipe open.  Write side dup'ed to stdout.
	 	 			 * - Exec the left command.
					 *
					 * Fill in code. */
					close(pipefds[0]);

					if ((dup2(pipefds[1], STDOUT_FILENO)) == -1){
                		perror("dup2 error");
                		exit(EXIT_FAILURE);
            		}
	  				close(pipefds[1]);

            		execvp(left_argv[0], left_argv);
                	
            		break;

        		/* Listening child. */
        		case 0 :

	  				/* - Redirect input of "child" through pipe.
					  * - Don't need write side of pipe. Read side dup'ed to stdin.
				  	 * - Exec command on right side of pipe and recursively deal with other pipes
					 *
					 * Fill in code. */
					close(pipefds[1]);

					// read from pipe
					if ((dup2(pipefds[0], STDIN_FILENO)) == -1){
               			perror("dup2 error");
                		exit(EXIT_FAILURE);
            		}

          			pipe_and_exec(&myArgv[pipe_argv_index+1]);
			}
	}
	perror("Couldn't fork or exec child process");
  	exit(errno);
}
