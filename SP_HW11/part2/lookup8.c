/*
 * lookup8 : does no looking up locally, but instead asks
 * a server for the answer. Communication is by Internet TCP Sockets
 * The name of the server is passed as resource. PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>

#include "dict.h"

int lookup(Dictrec * sought, const char * resource) {
	static int sockfd;
	static struct sockaddr_in server;
	struct hostent *host;
	static int first_time = 1;
	struct utsname *name;

	if (first_time) {        /* connect to socket ; resource is server name */
		first_time = 0;

		/* Set up destination address. */

		/* Fill in code. */

		/* Allocate socket.
		 * Fill in code. */
		
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd == -1){
			perror("socket error");
			exit(EXIT_FAILURE);
		}

		memset((char *)&server, '\0', sizeof(server));
		server.sin_family = AF_INET;
		host = gethostbyname(resource);
		server.sin_port = htons(PORT);
		memcpy((char *)&server.sin_addr, host->h_addr_list[0], host->h_length);

		
		/* Connect to the server.
		 * Fill in code. */

		if ((connect(sockfd, (struct sockaddr*)&server, sizeof(struct sockaddr_in))) == -1){
			perror("connect error");
			exit(EXIT_FAILURE);
		}


	}

	/* write query on socket ; await reply
	 * Fill in code. */

	if (write(sockfd, sought, sizeof(Dictrec)) == -1){
		perror("write error (client write to server)");
		exit(EXIT_FAILURE);
	}

	if (read(sockfd, sought, sizeof(Dictrec)) == -1){
		perror("receive error");
		exit(EXIT_FAILURE);
	}

	if (strcmp(sought->text,"Not Found!") != 0) {
		return FOUND;
	}

	return NOTFOUND;
}
