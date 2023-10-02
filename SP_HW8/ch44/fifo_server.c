#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stddef.h>
#include <limits.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <errno.h>

#define NAME_SIZE 4096

struct request {                /* Request (client --> server) */
    pid_t pid;                  /* PID of client */
    int seqLen;                 /* Length of desired sequence */
};

struct response {               /* Response (server --> client) */
    int seqNum;                 /* Start of sequence */
};


/*
write open but read don't open: server write to clientfifo, but client don't read clientfifo
            $./fifo_server block &
            $./fifo_bad_client
            $./fifo_client

solution:
            $./fifo_server nonblock &
            $./fifo_bad_client
            $./fifo_client

read open but write not open: server don't write to clientfifo, so client can't read block
            $./fifo_s_nowrite &
            $./fifo_client

solution: 
            $./fifo_s_nowrite &
            $./fifo_c_nonblock
*/

int main(int argc, char *argv[]){

    int serverFd, dummyFd, clientFd;
    char clientFifo[NAME_SIZE];
    struct request req;
    struct response resp;
    int seq_n = 0, flag, flag_2;


    if (argc != 2 || strcmp(argv[1], "--help") == 0){
        printf("usage: %s [block / nonblock]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "block") == 0){
        flag = O_WRONLY;
    }
    else if (strcmp(argv[1], "nonblock") == 0){
        flag = O_WRONLY | O_NONBLOCK;
    }
    else{
        printf("usage: %s [block / nonblock]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // get all permission
    umask(0);               
    // int mkfifo(const char *filename, mode_t mode);
    if (mkfifo("SERVER_FIFO", S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST){
        perror("server mkfifo error");
        exit(EXIT_FAILURE);
    }

    serverFd = open("SERVER_FIFO", O_RDONLY);
    if (serverFd == -1){
        perror("severFd open SERVER_FIFO error");
        exit(EXIT_FAILURE);
    }

    dummyFd = open("SERVER_FIFO", O_WRONLY);
    if (dummyFd == -1){
        perror("dummyFd open SERVER_FIFO error");
        exit(EXIT_FAILURE);
    }

    // ignore SIGPIPE signal
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR){
        perror("signal error");
        exit(EXIT_FAILURE);
    }

    for (;;){
        if (read(serverFd, &req, sizeof(struct request)) != sizeof(struct request)){
            perror("Error reading request; discarding");
            continue;
        }

        // snprintf: clientFd = "CLIENT_FIFO_" + pid
        snprintf(clientFifo, NAME_SIZE, "CLIENT_FIFO_%ld", (long)req.pid);
        clientFd = open(clientFifo, flag);
        if(clientFd == -1){
            perror("clienFd open clientFifo error");
            continue;
        }

        resp.seqNum = seq_n;
        if (write(clientFd, &resp, sizeof(struct response)) != sizeof(struct response)){
            fprintf(stderr, "Error writing to FIFO %s\n", clientFifo);
        }

        if (close(clientFd) == -1){
            perror("close");
            exit(EXIT_FAILURE);
        }

        seq_n += req.seqLen;

    }

}