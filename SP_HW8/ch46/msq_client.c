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



int main(int argc, char *argv[]){

    struct requestMsg req;
    struct responseMsg resp;
    int msg_l, qid;


    if (argc > 1 && strcmp(argv[1], "--help") == 0){
        printf("%s [seq-len]\n", argv[0]);
        exit(EXIT_FAILURE);
    }


    qid = msgget(SERVER_KEY, S_IWUSR | S_IRUSR);
    if (qid == -1){
        perror("msgget error");
        exit(EXIT_FAILURE);
    }


    /* send message */
    req.pid = getpid();
    req.mtype = 1;
    req.seqLen = (argc > 1) ? atoi(argv[1]) : 1;


    if (msgsnd(qid, &req, SIZE, 0) == -1){
        perror("msgsnd error");
        exit(EXIT_FAILURE);
    }


    /* get first reponse, select msgtyp = own pid*/
    msg_l = msgrcv(qid, &resp, SIZE, req.pid, 0);
    if (msg_l == -1){
        perror("msgrcv for msglen error");
        exit(EXIT_FAILURE);
    }


    printf("%d\n", resp.seqNum);

    exit(EXIT_SUCCESS);
}