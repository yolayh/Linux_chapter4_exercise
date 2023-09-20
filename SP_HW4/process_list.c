#include <dirent.h>     //readdir
#include <stdio.h>      //strtol
#include <stdlib.h>
#include <pwd.h>        //getpwnam
#include <sys/types.h>  //getpwnam getpid
#include <unistd.h>     //getpid
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>


#define MAX 4096


int main(int argc, char *argv[]){

    char *user_name, p;    
    struct passwd *pwd;    
    int my_uid, fd;           
    DIR *dir;              
    struct dirent *pid;    
    ssize_t rn;            
    char path[MAX];        
    char list[MAX];      
    static char delim[] = " \t\n():"; 
    char *token, *p_uid, *p_cmd;

    
    //check argv  //////////////////////////////////////////////////
    if (argc != 2 || strcmp(argv[1], "--help") == 0){
        printf("usage: %s [user name]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    user_name = argv[1];
    
    //find user id  //////////////////////////////////////////////////
    pwd = getpwnam(user_name);
    if (pwd == NULL){
        printf("get user id failed");
        exit(EXIT_FAILURE);
    }

    my_uid = pwd->pw_uid;

    // find all pid  //////////////////////////////////////////////////
    if((dir = opendir("/proc")) == NULL){
        perror("opendir fail");
        exit(EXIT_FAILURE);
    }
    else{
        while ((pid = readdir(dir)) != NULL){

            
            p = *(pid->d_name);
            if (isdigit(p)){

                snprintf(path, 4096, "/proc/%s/status", pid->d_name);
                fd = open(path, O_RDONLY);
                if (fd == -1){
                    perror("open pid status fail");
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
                    }
                    else if (strcmp(token, "Uid") == 0){
                        token = strtok(NULL, delim);
                        p_uid = token;
                        token = NULL;
                    }
                    else{
                        token = strtok(NULL, delim);
                    }
                }

                if (my_uid == atoi(p_uid)){
                    printf("Process ID = %s, Command name = %s\n", pid->d_name, p_cmd);
                }


                if (close(fd) == -1){
		            perror("Closing file error");
		            exit(EXIT_FAILURE);
	            }
        	} 
        }
    }
    
    closedir(dir);

    exit(EXIT_SUCCESS);

}

