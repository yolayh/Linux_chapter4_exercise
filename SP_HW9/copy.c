#include <sys/stat.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


int main(int argc, char *argv[]){

    void *addr_old, *addr_new;
    int fd_old, fd_new;
    struct stat buf;

    //check argument
    if (argc != 3 || strcmp(argv[1], "--help") == 0){
        printf("usage: %s [old file] [new file]\n", argv[0]);
    }

    // get the old file size
    fd_old = open(argv[1], O_RDWR);
    if (fd_old == -1){
        perror("open file error");
        exit(EXIT_FAILURE);
    }

    if (fstat(fd_old, &buf) == -1){
        perror("fstat error");
        exit(EXIT_FAILURE);
    }

    addr_old = mmap(NULL, buf.st_size, PROT_READ, MAP_SHARED, fd_old, 0);
    if (addr_old == MAP_FAILED){
        perror("mmap error");
        exit(EXIT_FAILURE);
    }


    // create new file
    fd_new = open(argv[1], O_RDWR | O_CREAT | O_TRUNC);
    if (fd_new == -1){
        perror("open file error");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd_new, buf.st_size) == -1){
        perror("ftruncate error");
        exit(EXIT_FAILURE);
    }

    addr_new = mmap(NULL, buf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_new, 0);

    if (addr_new == MAP_FAILED){
        perror("mmap error");
        exit(EXIT_FAILURE);
    }

    // copy
    memcpy (&addr_new, &addr_old, buf.st_size);

    if (close(fd_old) == -1){
        perror("close old file error");
        exit(EXIT_FAILURE);
    }

    if (close(fd_new) == -1){
        perror("close new file error");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}



