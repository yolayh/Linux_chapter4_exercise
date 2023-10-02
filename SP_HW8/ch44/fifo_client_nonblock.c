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

static char clientFifo[NAME_SIZE];

/* Invoked on exit to delete client FIFO */
static void removeFifo(void)
{
    unlink(clientFifo);
}

int main(int argc, char *argv[])
{
    int serverFd, clientFd;
    struct request req;
    struct response resp;

    if (argc > 1 && strcmp(argv[1], "--help") == 0){
        printf("%s [seq-len]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Create our FIFO (before sending request, to avoid a race) */
    req.pid = getpid();
    req.seqLen = (argc > 1) ? atoi(argv[1]) : 1;

    umask(0);                   /* So we get the permissions we want */
    snprintf(clientFifo, NAME_SIZE, "CLIENT_FIFO_%ld", (long)req.pid);
    if (mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST){
        perror("mkfifo clientFifo error");
        exit(EXIT_FAILURE);
    }

    if (atexit(removeFifo) != 0){
        perror("atexit");
        exit(EXIT_FAILURE);
    }

    // write request to server
    serverFd = open("SERVER_FIFO", O_WRONLY);
    if (serverFd == -1){
        perror("open SERVER_FIFO");
        exit(EXIT_FAILURE);
    }

    if (write(serverFd, &req, sizeof(struct request)) != sizeof(struct request)){
        perror("write request error");
        exit(EXIT_FAILURE);
    }

    /* Open our FIFO, read and display response */
    clientFd = open(clientFifo, O_RDONLY | O_NONBLOCK);
    if (clientFd == -1){
        perror("open clientFifo error");
        exit(EXIT_FAILURE);
    }

    if (read(clientFd, &resp, sizeof(struct response)) != sizeof(struct response)){
        perror("read clientFifo error");
        exit(EXIT_FAILURE);
    }

    printf("%d\n", resp.seqNum);
    exit(EXIT_SUCCESS);
}