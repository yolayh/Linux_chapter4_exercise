#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stddef.h>                     /* For definition of offsetof() */
#include <limits.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <string.h>

#define SERVER_NAME "/server_msq"
#define CLIENT_NAME "/client_msq"


static void removeQueue(){

    if (mq_unlink(CLIENT_NAME) == -1){
        perror("mq_unlink error");
        exit(EXIT_FAILURE);
    }

}

int main(int argc, char *argv[]){

    mqd_t server_mqd, client_mqd;
    struct mq_attr attr, *attrp;
    char *path;
    ssize_t msgLen;
    char buf[8192];


    if (argc != 2 || strcmp(argv[1], "--help") == 0){
        printf("%s pathname\n", argv[0]);
    }


    server_mqd = mq_open(SERVER_NAME, O_CREAT | O_RDWR, 0777, NULL);
    if (server_mqd < 0){
        perror("mq_open_server error");
        exit(EXIT_FAILURE);
    }
    

    if (atexit(removeQueue) != 0){
        perror("atexit_removeQueue error");
        exit(EXIT_FAILURE);
    }


    if (mq_send(server_mqd, argv[1], sizeof(char)*(strlen(argv[1])+1), 0) == -1){
        perror("mq_send error");
        exit(EXIT_FAILURE);
    }


    client_mqd = mq_open(CLIENT_NAME, O_RDWR | O_CREAT, 0777, NULL);
    if (client_mqd < 0){
        perror("mq_open_client error");
        exit(EXIT_FAILURE);
    }

    msgLen = mq_receive(client_mqd, buf, 8192, 0);
    


    printf("Received %ld bytes\n", msgLen);

    exit(EXIT_SUCCESS);
}