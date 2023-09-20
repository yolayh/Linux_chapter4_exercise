#include <dirent.h>     
#include <stdio.h>      
#include <stdlib.h>
#include <unistd.h>    
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>

#define MAX 4096

int main(int argc, char *argv[]){

    DIR *dir, *f_dir;              
    struct dirent *pid, *f_content;  
    char p;    
    int fd;            
    ssize_t rn;            
    char path[MAX], list[MAX], link[MAX];              
    static char delim[] = " \t\n:"; 
    char *token, *p_cmd, *file, *link_path;
    
//check argv  //////////////////////////////////////////////////
    if (argc != 2 || strcmp(argv[1], "--help") == 0){
        printf("usage: %s [file name]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

// find all pid  //////////////////////////////////////////////////
    if((dir = opendir("/proc")) == NULL){
        perror("opendir fail");
        exit(EXIT_FAILURE);
    }
    else{
        while ((pid = readdir(dir)) != NULL){
            
            p = *(pid->d_name);
            if (isdigit(p)){

                //find command name
                snprintf(path, 4096, "/proc/%s/status", pid->d_name);
                fd = open(path, O_RDONLY);
                if (fd == -1){
                    perror("open pid fd fail");
                    exit(EXIT_FAILURE); 
                }

                rn = read(fd, list, MAX);
                if (rn == -1){
                    perror("read problem");
                    exit(EXIT_FAILURE);
                }

                token = strtok(list, delim);
                if (token == NULL){
		            printf("NULL");
	            }

                while (token != NULL){
                    if (strcmp(token, "Name") == 0){
                        token = strtok(NULL, delim);
                        p_cmd = token;
                        token = NULL;
                    }
                    else{
                        token = strtok(NULL, delim);
                    }
                }

                if (close(fd) == -1){
		            perror("Closing file error");
		            exit(EXIT_FAILURE);
	            }

                //find all fd directory
                snprintf(path, 4096, "/proc/%s/fd", pid->d_name);

                if((f_dir = opendir(path)) == NULL){
                    perror("opendir fail");
                    printf("If reason is 'Permission denied', please try: sudo %s [file name]\n", argv[0]);
                    exit(EXIT_FAILURE);
                }
                else{
                    while ((f_content = readdir(f_dir)) != NULL){
                        p = *(f_content->d_name);
                        if (isdigit(p)){
                            snprintf(path, 4096, "/proc/%s/fd/%s", pid->d_name, f_content->d_name);
                            memset(link, 0, MAX);
                            if ((readlink(path, link, MAX)) == -1){
                                perror("readlink fail");
                                exit(EXIT_FAILURE);
                            }
                            else{
                                if((strcmp(link, argv[1])) == 0 ){
                                    printf("Process ID = %s, Command name = %s\n", pid->d_name, p_cmd);
                                }
                            }
                        }
                    }
                }
        	    
            }
        }
    }

    exit(EXIT_SUCCESS);

}

