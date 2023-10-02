#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stddef.h>
#include <limits.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <errno.h>
#include <unistd.h>


#define SIZE 10
#define SERVER_KEY 0x1aaaaaa1

struct requestMsg {
    long mtype;
    pid_t pid;
    int seqLen;
};

struct responseMsg {
    long mtype;                   /* type of message */
    int seqNum;     
};

static int qid;

static void removeMsgqueue(void){
    if (msgctl(qid, IPC_RMID, NULL) == -1){
        perror("msgctl error");
        exit(EXIT_FAILURE);
    }
}


int main(int argc, char *argv[])
{
    struct requestMsg req;
    struct responseMsg resp;
    ssize_t msgLen;
    int seq_num = 0;
    struct sigaction sa;


    /* Establish SIGCHLD handler to reap terminated children */

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR){
        perror("signal SIGPIPE SIG_IGN");
        exit(EXIT_FAILURE);
    }

    
    // setting terminate funtion
    if (atexit(removeMsgqueue) != 0){
        perror("atexit error");
    }


    // create message queue
    qid = msgget(SERVER_KEY, 0666 | IPC_CREAT);
    if (qid == -1){
        perror("msgget error");
        exit(EXIT_FAILURE);
    }

    

    /* Read requests and send repose by pid */

    for (;;) {

        msgLen = msgrcv(qid, &req, SIZE, 1, 0);
    
        if (msgLen == -1) {
            perror("msgrcv");
            break;                      /* ... so terminate loop */
        }

        
        resp.seqNum = seq_num;
        resp.mtype = req.pid;
        if (msgsnd(qid, &resp, SIZE, 0) == -1){
            perror("msgsnd error");
            break;
        }

        seq_num += req.seqLen;

 
    }


    exit(EXIT_SUCCESS);
}