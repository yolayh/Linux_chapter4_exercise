#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


FILE *my_popen(char **, char *);
int my_pclose(FILE *);
pid_t *pid_list;


int main(int argc, char *argv[]){

    FILE *fp;
    int status, readfile, cmd_n;
    char buf[4096];
    char *token, *type;
    char **cmd_list;
    static char delim[] = " "; 


    //check argument
    if (argc < 3 || strcmp(argv[1], "--help") == 0){
        printf("usage: %s [command -option] [r/w](read or write)\n", argv[0]);
    }

    //put command in a array
    token = strtok(argv[1], delim);
    if (token == NULL){
		printf("NULL");
	}

    cmd_n = 0;
    cmd_list = (char **)malloc((cmd_n+1) * sizeof(char*));
    cmd_list[cmd_n] = malloc((strlen(token)+1) * sizeof(char));
    
    while (token != NULL){
        cmd_list = (char **)realloc(cmd_list, (cmd_n+1) * sizeof(char*));
        cmd_list[cmd_n] = malloc((strlen(token)+1) * sizeof(char));
        strcpy(cmd_list[cmd_n], token);
        cmd_n++; 
        token = strtok(NULL, delim);
    }
    
    cmd_list[cmd_n] = NULL;

    pid_list = calloc(100, sizeof(pid_t));

    //
    
    fflush(stdout);
    fp = my_popen(cmd_list, argv[2]);
    
    if (fp == NULL){
        perror("popen failed");
        exit(EXIT_FAILURE);
    }

    while (fgets(buf, 4096, fp) != NULL){
        printf("%s", buf);
    }

    status = my_pclose(fp);
    if (status == -1){
        perror("pclose failed");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);

}


FILE *my_popen(char **command, char *type){

    FILE *fp;
    int pd[2], pid, fd, child_pid, opened;
    char buf;

    if (pipe(pd) == -1){
        perror("pipe error");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    switch(pid){

        case -1:
            perror("fork error");
            exit(EXIT_FAILURE);
        
        //***** child
        case 0:
            //***** type r: child write, parent read.
            if (strcmp(type, "r") == 0){

                close(pd[0]);

                if ((dup2(pd[1], STDOUT_FILENO)) == -1){
                    perror("dup2 error");
                    exit(EXIT_FAILURE);
                }
                
                for (int i = 0; i < 100; i++){
                    if (pid_list[i] > 0){
                        close(i);
                    }
                }

                if ((execvp(command[0], command)) == -1){
                    perror("execvp error");
                    exit(EXIT_FAILURE);
                }

                close(pd[1]);

                break;
            }
            //***** type w: child read, parent write.
            else if (strcmp(type, "w") == 0){

                if ((dup2(pd[0], STDIN_FILENO)) == -1){
                    perror("dup2 error");
                    exit(EXIT_FAILURE);
                }

                for (int i = 0; i < 100; i++){
                    if (pid_list[i] > 0){
                        close(i);
                    }
                }

                if ((execvp(command[0], command)) == -1){
                    perror("execvp error");
                    exit(EXIT_FAILURE);
                }

                close(pd[0]);

                break;
            }
            //***** type != r or w
            else{
                exit(EXIT_FAILURE);
            }

            break;

        //****** parent
        default:
            //***** type r: child write, parent read.
            if (strcmp(type, "r") == 0){
                close(pd[1]);

                fp = fdopen(pd[0], type);
                if (fp == NULL){
                    perror("fopen error");
                    exit(EXIT_FAILURE);
                }
     
            }

            //***** type w: child read, parent write.
            else if (strcmp(type, "w") == 0){
                close(pd[0]);

                fp = fdopen(pd[1], type);
                if (fp == NULL){
                    perror("fopen error");
                    exit(EXIT_FAILURE);
                }

            }

            //***** type != r or w
            else{
                printf("wrong type input, please type 'r' or 'w'\n");
                exit(EXIT_FAILURE);
            }


            break;
    }

        fd = fileno(fp);
        pid_list[fd] = pid;

        return fp;

}

// close pipe 
int my_pclose(FILE *file_pointer){

    int fd, status; 
    pid_t pid;

    fd = fileno(file_pointer);
    pid = pid_list[fd];

    if (pid == 0){
        printf("can't find the pid for pclose\n");
        exit(EXIT_FAILURE);
    }

    if(waitpid(pid, &status, 0) == -1){
        perror("waitpid error");
        exit(EXIT_FAILURE);
    }

    pid_list[fd] = 0;
    if (fclose(file_pointer) == EOF){
        perror("fclose error");
        exit(EXIT_FAILURE);
    }

    return status;

}
