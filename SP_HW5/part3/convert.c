/*
 * convert.c : take a file in the form 
 *  word1
 *  multiline definition of word1
 *  stretching over several lines, 
 * followed by a blank line
 * word2....etc
 * convert into a file of fixed-length records
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "dict.h"
#define BIGLINE 512

int main(int argc, char **argv) {
	FILE *in;
	FILE *out;        /* defaults */
	char line[BIGLINE];
	static Dictrec dr, blank;
	int noword = 1, len;
	
	/* If args are supplied, argv[1] is for input, argv[2] for output */
	if (argc==3) {
		if ((in = fopen(argv[1],"r")) == NULL){DIE(argv[1]);}
		if ((out = fopen(argv[2],"w")) == NULL){DIE(argv[2]);}	
	}
	else{
		printf("Usage: convert [input file] [output file].\n");
		return -1;
	}

	/* Main reading loop : read word first, then definition into dr */

	/* Loop through the whole file. */
	while (!feof(in)) {
		
		/* Create and fill in a new blank record.
		 * First get a word and put it in the word field, then get the definition
		 * and put it in the text field at the right offset.  Pad the unused chars
		 * in both fields with nulls.
		 */

		/* Read word and put in record.  Truncate at the end of the "word" field.
		 *
		 * Fill in code. */

		// memset(line, 0, BIGLINE);
		memset(blank.word, '\0', WORD);
		

		fgets(line, BIGLINE, in);
		if (strlen(line) == 1){   //超過兩行空白行
			break;
		}
		strcpy(blank.word, line);


		/* Read definition, line by line, and put in record.
		 *
		 * Fill in code. */

		
		memset(line, 0, BIGLINE);
		memset(blank.text, 0, TEXT);
		fgets(line, BIGLINE, in);
		len = strlen(line);
		line[len-1] = '\0';
		strcpy(blank.text, line);

		memset(line, 0, BIGLINE);
		fgets(line, BIGLINE, in);

		while (noword){
			if (strlen(line) == 1){
				noword = 0;
			}
			else{
				len = strlen(line);
				line[len-1] = '\0';
				strcat(blank.text, line);
				memset(line, 0, BIGLINE);
				fgets(line, BIGLINE, in);
			}
		}

		noword = 1;
		strcat(blank.text, "\n");

		/* Write record out to file.
		 *
		 * Fill in code. */
		fwrite(&blank.word, 32, 1, out); 		
		fwrite(&blank.text, 480, 1, out); 		
	}

	fclose(in);
	fclose(out);
	return 0;
}
