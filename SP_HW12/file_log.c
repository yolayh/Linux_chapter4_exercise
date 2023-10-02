#define _XOPEN_SOURCE 500
#include <sys/inotify.h>
#include <limits.h>
#include <string.h>
#include <ftw.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>

#define BUF_LEN  4096

int wd_num;
int inotifyFd;
char **dir_list;

int isDir(const char* fileName)
{
    struct stat path;
    stat(fileName, &path);
    return S_ISREG(path.st_mode);
}

static int find_subdir(const char *fpath, const struct stat *sb, int tflag,struct FTW *ftwbuf){
    
    int wd, flag, fd;
    char r[100];

    if (tflag == FTW_D){
        if (ftwbuf->level == 0){
            dir_list = (char **)malloc(1 * sizeof(char*));
            dir_list[wd_num-1] = malloc((strlen(fpath)+1)*sizeof(char));
            strcpy(dir_list[0], fpath);
        }
        if (ftwbuf->level != 0){
            // printf("add subdir:  %-40s\n", fpath);
            wd = inotify_add_watch(inotifyFd, fpath, IN_ALL_EVENTS);
            if (wd == -1){
                perror("create wd error");
                exit(EXIT_FAILURE);
            }
            else{
                // printf("wd=%d\n",wd);
                wd_num+=1;

                dir_list = (char **)realloc(dir_list, (wd_num) * sizeof(char*));
                dir_list[wd_num-1] = malloc((strlen(fpath)+1)*sizeof(char));
                strcpy(dir_list[wd_num-1], fpath);
            }
        }

        flag = O_CREAT | O_WRONLY | O_APPEND;
        fd = open("log_file", flag, 0766);

        if (fd == -1){
            perror("open error");
            exit(EXIT_FAILURE);
        }

        snprintf(r, 100, "(wd: %d watch -> %s)\n\n", wd, fpath);
        if (write(fd, r, strlen(r)) == -1){
            perror("write error");
            exit(EXIT_FAILURE);
        }
                
        if (close(fd) == -1){
            perror("close error");
            exit(EXIT_FAILURE);
        }
            
        
    }

    return 0;           /* To tell nftw() to continue */

}


static void inotify_event(struct inotify_event *i){

    int fd, flag, wd;
    char change[BUF_LEN];
    time_t now;
    char r[2048];
    char path[1024];

    flag = O_CREAT | O_WRONLY | O_APPEND;
    fd = open("log_file", flag, 0766);

    if (fd == -1){
        perror("open error");
        exit(EXIT_FAILURE);
    }

    time (&now);
    
    if (i->mask & IN_CREATE){
        snprintf(change, 4096, "%swd:%2d  **IN_CREATE**  name: %s\n\n", ctime(&now), i->wd, i->name);
        if ((write(fd, change, strlen(change))) == -1){
            perror("write error");
            exit(EXIT_FAILURE);
        }

        // add wd
        snprintf(path, 1024, "%s/%s", dir_list[(i->wd)-1], i->name);
        
        if (isDir(path) == 0){

            //printf("add new dir!! path=%s\n", path);
            wd = inotify_add_watch(inotifyFd, path, IN_ALL_EVENTS);
            if (wd == -1){
                perror("create wd error");
                exit(EXIT_FAILURE);
            }
            else{
                wd_num+=1;

                snprintf(r, 2048, "(wd: %d watch -> %s)\n\n", wd, path);
                if (write(fd, r, strlen(r)) == -1){
                    perror("write error");
                    exit(EXIT_FAILURE);
                }
                
                dir_list = (char **)realloc(dir_list, (wd_num) * sizeof(char*));
                dir_list[wd_num-1] = malloc((strlen(path)+1)*sizeof(char));
                strcpy(dir_list[wd_num-1], path);

            }
        }
    }

    if (i->mask & IN_DELETE){
        snprintf(change, 4096, "%swd:%2d  **IN_DELETE**  name: %s\n\n", ctime(&now), i->wd, i->name);
        if (write(fd, change, strlen(change)) == -1){
            perror("write error");
            exit(EXIT_FAILURE);
        }
    }

    if (i->mask & IN_MOVED_FROM){
        snprintf(change, 4096, "%swd:%2d  **IN_MOVED_FROM**  name: %s\n\n", ctime(&now), i->wd, i->name);
        if (write(fd, change, strlen(change)) == -1){
            perror("write error");
            exit(EXIT_FAILURE);
        }
    }

    if (i->mask & IN_MOVED_TO){
        snprintf(change, 4096, "%swd:%2d  **IN_MOVED_TO**  name: %s\n\n", ctime(&now), i->wd, i->name);
        if (write(fd, change, strlen(change)) == -1){
            perror("write error");
            exit(EXIT_FAILURE);
        }
    }

    //check wd rm
    
    // if (i->mask & IN_DELETE_SELF){
    //     printf("wd=%d\n", i->wd);
    // }

    // if (i->mask & IN_IGNORED){
    //     printf("wd-%d removed!\n", i->wd);
    // }


    if (close(fd) == -1){
        perror("close error");
        exit(EXIT_FAILURE);
    }
    
}


int main(int argc, char *argv[]){

    int wd, fd, flag;
    char buf[BUF_LEN];
    ssize_t numRead;
    struct inotify_event *event;
    char *p;
    char r[1024];


    if (argc < 2 || strcmp(argv[1], "--help") == 0){
        printf("usage: %s [directory path]\n", argv[0]);
        exit(EXIT_FAILURE);
    }


    // inotify
    inotifyFd = inotify_init();
    if (inotifyFd == -1){
        perror("inotify_init error");
        exit(EXIT_FAILURE);
    }

    wd = inotify_add_watch(inotifyFd, argv[1], IN_ALL_EVENTS);
    if (wd == -1){
        perror("inotify_add_watch error");
        exit(EXIT_FAILURE);
    }

    wd_num += 1;

    if (nftw(argv[1], find_subdir, 20, FTW_PHYS) == -1){
        perror("nftw error");
        exit(EXIT_FAILURE);
    }

    // for (int i = 0; i < wd_num; i++ ){
    //     printf("%s\n", dir_list[i]);
    // }


    for (;;){

        numRead = read(inotifyFd, buf, BUF_LEN);
        
        if (numRead == 0){
            printf("read from inotify fd returned 0");
        }

        if (numRead == -1){
            perror("read error");
            exit(EXIT_FAILURE);
        }

        for (p = buf; p < buf + numRead; ){
            event = (struct inotify_event *) p;
            inotify_event(event);

            p += sizeof(struct inotify_event) + event->len;
        }

     
    }

    for (int i = 1; i<=wd_num; i++){
        printf("remove: %d\n", i);
        if (inotify_rm_watch(inotifyFd, i) == -1){
            perror("remove watch error");
            exit(EXIT_FAILURE);
        }
    }

    
    exit(EXIT_SUCCESS);



}
