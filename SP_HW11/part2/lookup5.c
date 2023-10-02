/*
 * lookup5 : local file ; setup a memory map of the file
 *           Use bsearch. We assume that the words are already
 *           in dictionary order in a file of fixed-size records
 *           of type Dictrec
 *           The name of the file is what is passed as resource
 */

#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "dict.h"

/*
 * This obscure looking function will be useful with bsearch
 * It compares a word with the word part of a Dictrec
 */

int dict_cmp(const void *a,const void *b) {
    return strcmp((char *)a,((Dictrec *)b)->word);
}

int lookup(Dictrec * sought, const char * resource) {
	static Dictrec * table;
	static int numrec;
	Dictrec * found;
	static int first_time = 1;
	void *addr;

	if (first_time) {  /* table ends up pointing at mmap of file */
		int fd;
		long filsiz;

		first_time = 0;

		/* Open the dictionary.
		 * Fill in code. */
		fd = open(resource, O_RDONLY);
		if (fd == -1){
			perror("open error");
			exit(EXIT_FAILURE);
		}

		/* Get record count for building the tree. */
		filsiz = lseek(fd,0L,SEEK_END);
		numrec = filsiz / sizeof(Dictrec);

		addr = mmap(NULL, filsiz, PROT_READ, MAP_SHARED, fd, 0);
		if (addr == MAP_FAILED){
        	perror("mmap error");
        	exit(EXIT_FAILURE);
    	}
		/* mmap the data.
		 * Fill in code. */
		close(fd);
	}
    
	/* search table using bsearch
	 * Fill in code. */
	found = bsearch (&sought->word, addr, numrec, sizeof(Dictrec), dict_cmp);

	if (found) {
		strcpy(sought->text,found->text);
		return FOUND;
	}
	else{
		strcpy(sought->text, "Not Found!");
		return NOTFOUND;
	}
}
