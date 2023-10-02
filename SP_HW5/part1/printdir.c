#include <sys/param.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(){

    char *dir;
    long path_max_len = pathconf(".", _PC_PATH_MAX);

    dir = getcwd((char*)NULL, path_max_len+1);

    if(dir == NULL){
        perror("getcwd error");
        exit(EXIT_FAILURE);
    }

    printf("%s\n", dir);
    free(dir);

    exit(EXIT_SUCCESS);
    
}