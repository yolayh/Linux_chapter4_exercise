/*
 * isock_server : listen on an internet socket ; fork ;
 *                child does lookup ; replies down same socket
 * argv[1] is the name of the local datafile
 * PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>


#include "dict.h"

int main(int argc, char **argv) {
	static struct sockaddr_in server;
	int sd,cd,n;
	Dictrec tryit;

	if (argc != 2) {
		fprintf(stderr,"Usage : %s <datafile>\n",argv[0]);
		exit(1);
	}

	/* Create the socket.
	 * Fill in code. */
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket error");
		exit(EXIT_FAILURE);
	}

	/* Initialize address.
	 * Fill in code. */

	/* Name and activate the socket.
	 * Fill in code. */

	memset((char *)&server,'\0', sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = INADDR_ANY;


	if (bind(sd, (struct sockaddr *)&server, sizeof(server))== -1){
		perror("bind error");
		exit(EXIT_FAILURE);
	}

	if (listen(sd, 128) == -1){
			perror("listen error");
			exit(EXIT_FAILURE);
	}

	/* main loop : accept connection; fork a child to have dialogue */
	for (;;) {

		/* Wait for a connection.
		 * Fill in code. */

		cd = accept(sd, NULL, NULL);
		if (cd == -1){
			perror("accept error");
			exit(EXIT_FAILURE);
		}

		/* Handle new client in a subprocess. */
		switch (fork()) {
			case -1 : 
				DIE("fork");
			case 0 :
				close (sd);	/* Rendezvous socket is for parent only. */
				/* Get next request.
				 * Fill in code. */
				while (read(cd, &tryit, sizeof(tryit)) > 0) {
									
					/* Lookup the word , handling the different cases appropriately */
					switch(lookup(&tryit,argv[1]) ) {
						/* Write response back to the client. */
						case FOUND:
							if ((write(cd, &tryit, sizeof(Dictrec))) == -1){
								perror("write error (server found)");
								exit(EXIT_FAILURE);
							}

							break;

						case NOTFOUND:
							/* Fill in code. */
							if ((write(cd, &tryit, sizeof(Dictrec))) == -1){
								perror("write error (server not found)");
								exit(EXIT_FAILURE);
							}
							
							break;
						
						case UNAVAIL:
							DIE(argv[1]);
					} /* end lookup switch */
				} /* end of client dialog */
				exit(0); /* child does not want to be a parent */

			default :
				close(cd);
				break;
		} /* end fork switch */
	} /* end forever loop */
} /* end main */
