#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 100

int main(int argc, char *argv[]){
	
	int flag, opt, fd, rn;
	char buffer[SIZE];
	const char *file;
	ssize_t read_content, written_content;
	
	flag = O_CREAT | O_WRONLY | O_TRUNC;	
	file = argv[1];

	//getopt
	while((opt = getopt(argc, argv, "a:")) != -1){
		switch(opt){
			case 'a':
				flag = O_CREAT | O_WRONLY | O_APPEND;
				file = optarg;
				break;
			default:
				printf("usage: %s [-a] file\n", argv[0]);
				exit(EXIT_FAILURE);
				
		}
	}
	

	//open
	mode_t permission = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
	fd = open(file, flag, permission);
	if(fd == -1){
		perror("Opening file error");
		exit(EXIT_FAILURE);
	}

	//read from terminal and write to file
	while(rn = read(STDIN_FILENO, buffer, SIZE)){
		if (rn == -1){
			perror("Reading file error");
			exit(EXIT_FAILURE);
		}
		else{
			write(STDOUT_FILENO, buffer, rn);	
			written_content = write(fd, buffer, rn);
			if (written_content == -1){
				perror("Writing file error");
				exit(EXIT_FAILURE);
		        }
        	} 
	}

	
	//close
	close(fd);
	if (close(fd) == -1){
		perror("Closing file error");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
		
	
}	


