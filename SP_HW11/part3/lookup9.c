/*
 * lookup9 : does no looking up locally, but instead asks
 * a server for the answer. Communication is by Internet UDP Sockets
 * The name of the server is passed as resource. PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include "dict.h"
#include <string.h>
#include <unistd.h>

int lookup(Dictrec * sought, const char * resource) {
	static int sockfd;
	static struct sockaddr_in server, client;
	struct hostent *host;
	static int first_time = 1;

	if (first_time) {  /* Set up server address & create local UDP socket */
		first_time = 0;

		/* Set up destination address.
		
		 * Fill in code. */

		sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		if (sockfd == -1){
			perror("sockfd");
			exit(EXIT_FAILURE);
		}

		// bind arbitrary return address
		memset((char *)&client,'\0', sizeof(client));
		client.sin_family = AF_INET;
		client.sin_addr.s_addr = htonl(INADDR_ANY);
		client.sin_port = htons(0);

		if (bind(sockfd, (struct sockaddr *)&client, sizeof(client))== -1){
			perror("clinet bind client error");
			exit(EXIT_FAILURE);
		}

		// bind server

		memset((char *)&server,'\0', sizeof(server));
		server.sin_family = AF_INET;
		server.sin_port = htons(PORT);

		/* Allocate a socket.
		 * Fill in code. */
		
		
		host = gethostbyname(resource);
		memcpy((char *)&server.sin_addr, host->h_addr_list[0], host->h_length);		


		// if (bind(sockfd, (struct sockaddr *)&server, sizeof(server))== -1){
		// 	perror("clinet bind server error");
		// 	exit(EXIT_FAILURE);
		// }


	}

	/* Send a datagram & await reply
	 * Fill in code. */
	int size = sizeof(server);

	if ((sendto(sockfd, sought, sizeof(Dictrec), 0, (struct sockaddr*)&server, size)) == -1){
		perror("sendto error");
		exit(EXIT_FAILURE);
	}

	if ((recvfrom(sockfd, sought, sizeof(Dictrec), 0,  (struct sockaddr*)&server, &size)) == -1){
		perror("recvfrom error");
		exit(EXIT_FAILURE);
	}


	if (strcmp(sought->text,"Not Found!") != 0) {
		return FOUND;
	}

	return NOTFOUND;
}
