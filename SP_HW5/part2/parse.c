/*
 * parse.c : use whitespace to tokenise a line
 * Initialise a vector big enough
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "shell.h"

/* Parse a commandline string into an argv array. */
char ** parse(char *line) {

  	static char delim[] = " \t\n"; /* SPACE or TAB or NL */
  	int count = 0;
  	char * token;
  	char **newArgv;

  	/* Nothing entered. */
  	if (line == NULL || strcmp(line,"\n")==0) {
    	return NULL;
  	}

  	/* Init strtok with commandline, then get first token.
     * Return NULL if no tokens in line.
	 *
	 * Fill in code.
     */
	token = strtok(line, delim);

	if (token == NULL){
		printf("NULL\n");
	}
	
 	/* Create array with room for first token.
  	 *
	 * Fill in code.
	 */
	newArgv = (char **)malloc((count+1) * sizeof(char*));
	memset(newArgv, 0, sizeof(char*));
	newArgv[count] = malloc((strlen(token)+1) * sizeof(char));
	memset(newArgv[count], 0, (strlen(token)+1) * sizeof(char));


  	/* While there are more tokens...
	 *
	 *  - Get next token.
	 *	- Resize array.
	 *  - Give token its own memory, then install it.
	 * 
  	 * Fill in code.
	 */
	while (token != NULL){
		strcpy(newArgv[count], token);
		count ++;
		newArgv = (char **)realloc(newArgv, (count+1) * sizeof(char*));
		newArgv[count] = malloc((strlen(token)+1)*sizeof(char));
		token = strtok(NULL, delim);
	}

  	/* Null terminate the array and return it.
	 *
  	 * Fill in code.
	 */

	newArgv[count] = NULL;

  	return newArgv;
}


/*
 * Free memory associated with argv array passed in.
 * Argv array is assumed created with parse() above.
 */
void free_argv(char **oldArgv) {

	int i = 0;

	/* Free each string hanging off the array.
	 * Free the oldArgv array itself.
	 *
	 * Fill in code.
	 */

	while (oldArgv[i] != NULL){
		free(oldArgv[i]);
		i++;
	}

	free(oldArgv);
}
