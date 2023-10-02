#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAXSIZE 4096

int main(int argc, char *argv[]){
    
    int fd;
    char content[MAXSIZE];
    ssize_t rn;
    char *file;

    //check argv  //////////////////////////////////////////////////
    if (argc != 2 || strcmp(argv[1], "--help") == 0){
        printf("Usage: mycat filename\n");
        exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_RDONLY);

    if (fd == -1){
        perror("open error");
        exit(EXIT_FAILURE);
    }

    rn = read(fd, content, MAXSIZE);
    if (rn == -1){
        perror("read error");
        exit(EXIT_FAILURE);
    }
    
    if ((write(STDOUT_FILENO, content, rn)) == -1){
        perror("write error");
        exit(EXIT_FAILURE);
    }

    if (close(fd) == -1){ 
        perror("close error");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);

}