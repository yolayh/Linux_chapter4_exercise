/*
 * redirect_out.c   :   check for >
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#define STD_OUTPUT 1
#define STD_INPUT  0

/*
 * Look for ">" in myArgv, then redirect output to the file.
 * Returns 0 on success, sets errno and returns -1 on error.
 */
int redirect_out(char ** myArgv) {
	int i = 0;
  	int fd;

  	/* search forward for >
  	 * Fill in code. */
	if (*myArgv == NULL)
    	return -1;
	
	while(myArgv[i] != NULL){

		if (strcmp(myArgv[i], ">") == 0){	/* found ">" in vector. */

		   /* 1) Open file.
			* 2) Redirect to use it for output.
			* 3) Cleanup / close unneeded file descriptors.
			* 4) Remove the ">" and the filename from myArgv.
			*
			* Fill in code. */
			fd = open(myArgv[i+1], O_CREAT|O_WRONLY|O_TRUNC, 0660);

			if (fd == -1){
				perror("redirect_out open error");
				return -1;
			}

			if ( dup2(fd, STDOUT_FILENO) == -1){
				perror("redirect_out dup2 error");
				return -1;
			}

			if (close(fd) == -1){
				perror("redirect_out close error");
				return -1;
			}

			myArgv[i] = NULL;
			myArgv[i+1] = NULL;

		}

		i++;
	}

  	return 0;
}
