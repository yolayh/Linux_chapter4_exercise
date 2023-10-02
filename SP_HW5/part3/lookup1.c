/*
 * lookup1 : straight linear search through a local file
 * 	         of fixed length records. The file name is passed
 *	         as resource.
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "dict.h"

int lookup(Dictrec * sought, const char * resource) {
	Dictrec dr;
	static FILE * in;
	static int first_time = 1;
	int len;

	if (first_time) { 
		first_time = 0;
		/* open up the file
		 *
		 * Fill in code. */
		in = fopen(resource, "r");
		if (in == NULL){
			DIE(resource);
		}
	}

	/* read from top of file, looking for match
	 *
	 * Fill in code. */

	rewind(in);

	while(!feof(in)) {

		fgets(dr.word, 32, in);
		if (strlen(dr.word) == 1){
			break;
		}
		fseek(in, 32-strlen(dr.word), SEEK_CUR);
		fgets(dr.text, 480, in);
		fseek(in, 480-strlen(dr.text), SEEK_CUR);

		len = strlen(dr.word);
		dr.word[len-1] = '\0';
		
		if (strcmp(dr.word, sought->word) == 0){
			len = strlen(dr.text);
			dr.text[len-1] = '\0';
			strcpy(sought->text, dr.text);
			return FOUND;
		}

	}

	return NOTFOUND;
}
