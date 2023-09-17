/*
 * run_command.c :    do the fork, exec stuff, call other functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "shell.h"


void run_command(char **myArgv) {
    pid_t pid;
    int stat;

    int b;
    b = is_background(myArgv);

    /* Create a new child process.
     * Fill in code.
	 */

    pid = fork();

    switch (pid) {

        /* Error. */
        case -1 :
            perror("fork");
            exit(errno);

        /* Parent. */
        default :
            /* Wait for child to terminate.
             * Fill in code.
			 */
            if (!b){
                wait(&stat);
            }

            /* Optional: display exit status.  (See wstat(5).)
             * Fill in code.
			 */
            
            printf("exit status: %d\n",WEXITSTATUS(stat));
            
            return;

        /* Child. */
        case 0 :
            /* Run command in child process.
             * Fill in code.
			 */
            // is_background(myArgv);
            execvp(myArgv[0], myArgv);

            /* Handle error return from exec */
			exit(errno);
    }
}
