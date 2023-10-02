/*
 * usock_server : listen on a Unix socket ; fork ;
 *                child does lookup ; replies down same socket
 * argv[1] is the name of the local datafile
 * PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include "dict.h"
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
    struct sockaddr_un server, client;
    int sd,cd,n;
    Dictrec tryit;

    if (argc != 3) {
      fprintf(stderr,"Usage : %s <dictionary source>"
          "<Socket name>\n",argv[0]);
      exit(errno);
    }

    /* Setup socket.
     * Fill in code. */
	if ((sd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
		perror("socket error");
		exit(EXIT_FAILURE);
	}
    
    /* Initialize address.
     * Fill in code. */

	server.sun_family = AF_UNIX;
	strcpy(server.sun_path, argv[2]);
	unlink(server.sun_path);

    /* Name and activate the socket.
     * Fill in code. */

	if ((bind(sd, (struct sockaddr*)&server, sizeof(struct sockaddr_un))) == -1){
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
					// printf("server receive %s\n", tryit.word); // receive success
					/* Lookup request. */
					switch(lookup(&tryit,argv[1])) {
						/* Write response back to client. */
						case FOUND: 
							/* Fill in code. */
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

				/* Terminate child process.  It is done. */
				exit(0);

			/* Parent continues here. */
			default :
				close(cd);
				break;
		} /* end fork switch */
    } /* end forever loop */
} /* end main */
