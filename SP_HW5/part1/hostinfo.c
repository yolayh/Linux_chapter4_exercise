#include <sys/utsname.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF 1024

int main(){

    long id;
    struct utsname uts;
    
    if (uname(&uts) == -1){
        perror("uname error");
        exit(EXIT_FAILURE);
    }

    printf("hostname: %s\n", uts.nodename);
    printf("%s\n", uts.release);
    
    id = gethostid();
    printf("hostid: %lx\n", id);

    exit(EXIT_SUCCESS);
}
