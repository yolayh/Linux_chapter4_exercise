#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SIZE 1024

int main(int argc, char *argv[]){

	int in_file, out_file, flag;
	mode_t permission;
	ssize_t nread, nwrite;
	char buf[SIZE];

	if (argc != 3 || strcmp(argv[1], "--help") == 0){
		printf("usage: %s old_file new_file\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	in_file = open(argv[1], O_RDONLY);
	if (in_file == -1){
		perror("Open fail");
		exit(EXIT_FAILURE);
	}

	flag = O_CREAT | O_WRONLY | O_TRUNC;
	permission = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

	out_file = open(argv[2], flag, permission);
	if (out_file == -1){
		perror("Open fail");
		exit(EXIT_FAILURE);
	}

	while ((nread = read(in_file, buf, SIZE)) > 0){

		if ((nwrite = write(out_file, buf, nread)) != nread){
			perror("Copy incomplete");
			exit(EXIT_FAILURE);
		}

		if (nwrite == -1){
			perror("Write fail");
			exit(EXIT_FAILURE);
		}
		
	}
		        

	if (nread == -1){
		perror("Read fail");
		exit(EXIT_FAILURE);
	}

	if (close(in_file) == -1){
		perror("Close fail");
		exit(EXIT_FAILURE);
	}
	if (close(out_file) == -1){
		perror("Close fail");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);

}
