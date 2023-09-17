#include <unistd.h>             // getopt, lseek, getpagesize
#include <sys/types.h>          // lseek
#include <sys/stat.h> 
#include <string.h>
#include <stdio.h>              // printf
#include <fcntl.h>              // open
#include <stdlib.h>             // exit


int main(int argc, char *argv[]){

    int option, fd, page;
    extern char*optarg;
    extern int optind;      // argv index
    off_t offset = 10, file_size;    // default value = 10
    ssize_t read_n, to_write;
    size_t length; 

    //check argument
    if (argc < 2 || argc > 4 || argc == 3 || strcmp(argv[1], "--help") == 0){
        printf("usage: %s [-n number] file\n", argv[0]);
    }


    //getopt
    while((option = getopt(argc, argv, "n:")) != -1){
		switch(option){
			case 'n':
				offset = atoi(optarg);
				break;
			default:
				printf("usage: %s [-n number] file\n", argv[0]);
				exit(EXIT_FAILURE);
		}
	}

    
    //get file size
    struct stat st;

    if (stat(argv[optind], &st) == 0){
        file_size = st.st_size;
    }
    else{
        perror("get file size fault");
        exit(EXIT_FAILURE);
    }

    //
    length = getpagesize();
    page = file_size / length + 1;
    length = length * page;

    char buf[length];

    //open
    fd = open(argv[optind], O_RDONLY);
    if (fd == -1){
        perror("open file error");
        exit(EXIT_FAILURE);
    }
    
    //read all file
    read_n = read(fd, buf, length);            
    if (read_n == -1){
        perror("read failed");
        exit(EXIT_FAILURE);
    }

    //count line from file end and count the byte we need
    to_write = -1;
    unsigned long i = read_n-1;

    while((offset+1)>0){
        
        if (buf[i] == '\n'){
            offset--;
        }
        to_write++;
        i--;
    }
 
    if (lseek(fd, -to_write, SEEK_END) == -1){
        perror("lseek failed");
        exit(EXIT_FAILURE);
    }
        
    if (read(fd, buf, to_write) == -1){
        perror("read failed");
        exit(EXIT_FAILURE);
    }

    if ((write(STDOUT_FILENO, buf, to_write)) == -1){
        perror("write failed");
	    exit(EXIT_FAILURE);
    }

    if (close(fd) == -1){
	    perror("Close error");
	    exit(EXIT_FAILURE);
    }
		
    exit(EXIT_SUCCESS);

}
