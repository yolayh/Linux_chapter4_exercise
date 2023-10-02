/*
 *  pipe_present.c :  check for |
 */

#include <stdio.h>
#include <string.h>
#include "shell.h"

/*
 * Return index offset into argv of where "|" is,
 * -1 if in an illegal position (first or last index in the array),
 * or 0 if not present.
 */
int pipe_present(char ** myCurrentArgv) {
	int index = 0;

	if (*myCurrentArgv == NULL)
    	return -1;

  	/* Search through myCurrentArgv for a match on "|". */

	while(myCurrentArgv[index] != NULL){
		if (strcmp(myCurrentArgv[index], "|") == 0){     /* found "<" in vector. */

  			if (index == 0){ /* At the beginning or at the end. */ 
    			return -1;
  			} 
			else if(myCurrentArgv[index+1] == NULL) /* Off the end. */ {
    			return 0;
  			} 
			else {
    			/* In the middle. */
    			return index;
  			}
		}
		index++;
	}
}
