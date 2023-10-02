/*
 * udp_server : listen on a UDP socket ;reply immediately
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
	static struct sockaddr_in server,client;
	int sockfd, siz;
	Dictrec dr, *tryit = &dr;

	if (argc != 2) {
		fprintf(stderr,"Usage : %s <datafile>\n",argv[0]);
		exit(errno);
	}

	/* Create a UDP socket.
	 * Fill in code. */

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1){
		perror("socket error");
		exit(EXIT_FAILURE);
	}

	/* Initialize address.
	 * Fill in code. */
	memset((char *)&server,'\0', sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(PORT);
	
	/* Name and activate the socket.
	 * Fill in code. */

	if (bind(sockfd, (struct sockaddr *)&server, sizeof(server))== -1){
		perror("server bind error");
		exit(EXIT_FAILURE);
	}

	for (;;) { /* await client packet; respond immediately */

		siz = sizeof(client); /* siz must be non-zero */

		/* Wait for a request.
		 * Fill in code. */
		
		while ((recvfrom(sockfd, tryit, sizeof(Dictrec), 0, (struct sockaddr*)&client, &siz) > 0)) {
			// printf("server receive and find %s, %s \n", tryit->word, tryit->text);
			/* Lookup request and respond to user. */
			switch(lookup(tryit,argv[1]) ) {
				
				case FOUND: 
					/* Send response.
					 * Fill in code. */
					if (sendto(sockfd, tryit, sizeof(Dictrec), 0, (struct sockaddr*)&client, siz) == -1){
						perror("sendto error");
						exit(EXIT_FAILURE);
					}
					break;
				case NOTFOUND : 
					if (sendto(sockfd, tryit, sizeof(Dictrec), 0, (struct sockaddr*)&client, siz) == -1){
						perror("sendto error");
						exit(EXIT_FAILURE);
					}
					/* Send response.
					 * Fill in code. */
					break;
				case UNAVAIL:
					DIE(argv[1]);
			} /* end lookup switch */
		} /* end while */
	} /* end forever loop */
} /* end main */
