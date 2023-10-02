/*
    usage:
            ./mq_server &
            ./mq_client [file]

*/


#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stddef.h>                     /* For definition of offsetof() */
#include <limits.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define SERVER_NAME "/server_msq"
#define CLIENT_NAME "/client_msq"


static void             /* SIGCHLD handler */
grimReaper(int sig)
{
    int savedErrno;

    savedErrno = errno;                 /* waitpid() might change 'errno' */
    while (waitpid(-1, NULL, WNOHANG) > 0)
        continue;
    errno = savedErrno;
}


static void removeQueue(){

    if (mq_unlink(SERVER_NAME) == -1){
        perror("mq_unlink error");
        exit(EXIT_FAILURE);
    }

}

int main(int argc, char *argv[]){

    mqd_t server_mqd, client_mqd;
    struct mq_attr attr, *attrp;
    char buf[8192], file_read[8192];
    pid_t pid;
    ssize_t msgLen, readn;
    int fd;

    server_mqd = mq_open(SERVER_NAME, O_RDWR | O_CREAT, 0777, NULL);
    if (server_mqd < 0){
        perror("mq_open error");
        exit(EXIT_FAILURE);
    }

    /* Establish SIGCHLD handler to reap terminated children */
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grimReaper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1){
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    if (atexit(removeQueue) != 0){
        perror("atexit_removeQueue error");
        exit(EXIT_FAILURE);
    }

    /* Read requests, handle each in a separate child process */

    for (;;) {

        mq_receive(server_mqd, buf, 8192, NULL);

        fd = open(buf, O_RDWR);
        if (fd == -1){
            perror("open error");
            exit(EXIT_FAILURE);
        }
        readn = read(fd, file_read, 8192);
        if(readn == -1){
            perror("read error");
            exit(EXIT_FAILURE);
        }

        client_mqd = mq_open(CLIENT_NAME, O_CREAT | O_RDWR, 0777, NULL);
        if (client_mqd < 0){
            perror("mq_open client error");
            exit(EXIT_FAILURE);
        }
 
        if (mq_send(client_mqd, file_read, readn, 0) == -1){
            perror("mq_send error");
            exit(EXIT_FAILURE);
        }

        if (close(fd)==-1){
            perror("close fd error");
            exit(EXIT_FAILURE);
        }
        if (mq_close(client_mqd)==-1){
            perror("mq_close");
            exit(EXIT_FAILURE);
        }


    }


    if (mq_unlink(SERVER_NAME) == -1){
        perror("mq_unlink error");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}