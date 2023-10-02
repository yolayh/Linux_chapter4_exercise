#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>


#define SHM_FILE "/sharememory_file"


struct shm {

    int data;
    int buf;
    char **message_buffer;
    int received_num;

};

void my_handler(int sig, siginfo_t *info, void *pvo){

    struct shm *addrr;
    int receive_data = info->si_int;
    // printf("pid-%d, get data-%d\n", getpid(), receive_data);
    
    int fdd = shm_open(SHM_FILE, O_RDWR, 0666);
    if (fdd == -1){
        perror("shm_open error");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fdd, 8192) == -1){
        perror("ftruncate error");
        exit(EXIT_FAILURE);
    }

    addrr = mmap(NULL, 8192, PROT_READ | PROT_WRITE, MAP_SHARED, fdd, 0);
    if (addrr == MAP_FAILED){
        perror("mmap error");
        exit(EXIT_FAILURE);
    }

    if (close(fdd) == -1){
        perror("close fd error");
        exit(EXIT_FAILURE);
    }

    
    addrr->received_num += 1;
    printf("%s\n", addrr->message_buffer[-1]);
 
    //int find = receive_data % addrr->buf;
    //printf("pid-%d receive: This is message %d\n", getpid(), receive_data);
    //printf("m-> %s\n", addrr->message_buffer[find]);


}

void consume(){

    struct sigaction act;
    union sigval mysigval;
    act.sa_sigaction = &my_handler;
    sigemptyset(&act.sa_mask);
    while (1);

}


int main(int argc, char **argv){

    int fd, pid, seq;
    int data_num, rate, consumer, buf_size;
    struct shm *addr;
    char message[80];
    int pid_list[consumer];
    

    if (argc != 5 || strcmp(argv[1], "--help") == 0){
        printf("%s [number] [rate] [consumer] [buffer size]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    data_num = atoi(argv[1]);
    rate = atoi(argv[2]);
    consumer = atoi(argv[3]);
    buf_size = atoi(argv[4]);


    // setting signal
    struct sigaction act;
    union sigval mysigval;
    act.sa_sigaction = &my_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO; 
    if(sigaction(SIGUSR1, &act, NULL) == -1){
        perror("sigaction error");
        exit(EXIT_FAILURE);
    }

    
    // create shared memory
    fd = shm_open(SHM_FILE, O_CREAT | O_RDWR, 0666);
    if (fd == -1){
        perror("shm_open error");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, 4096) == -1){
        perror("ftruncate error");
        exit(EXIT_FAILURE);
    }

    addr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED){
        perror("mmap error");
        exit(EXIT_FAILURE);
    }

    if (close(fd) == -1){
        perror("close fd error");
        exit(EXIT_FAILURE);
    }

    // setting shared memory
    addr->buf = buf_size;
    addr->data = data_num;
    addr->received_num = 0;
    addr->message_buffer = (char**)malloc((data_num)*sizeof(char*));

    for (int i = 0; i < buf_size; i++){
        //addr->message_buffer[i] = (char*)malloc(80*sizeof(char));
        addr->message_buffer[i] = (char*)malloc(80);
    }

    pid_t main_pid = getpid();

    // create consumer
    for (int i = 0; i < consumer; i++){
       
        if (getpid() == main_pid){
            pid_t child = fork();

            if (child == -1){
                perror("fork error");
                exit(EXIT_FAILURE);
            }
            // child process
            else if (child == 0){   
                // printf("I am child: %d\n", getpid());
                consume();
            }
            else{
                pid_list[i] = child;
                // printf("finish fork\n");
            }
        }
    }


    // write data to shared memory
    for (int i = 0; i < data_num; i++){
        snprintf(message, 80, "This is message %d", i);

    // write to shared memory
    if (i == 0){
        memcpy(addr->message_buffer[i], message, 80);
        addr->message_buffer[i][79] = '\0';
        }
        else{
            seq = (i % buf_size);
            memcpy(addr->message_buffer[seq], message, 80);
            addr->message_buffer[seq][79] = '\0';
        }
        mysigval.sival_int = i;
        // send signal to consumer
        for (int j = 0; j < consumer; j++){

            if(sigqueue(pid_list[j], SIGUSR1, mysigval) == -1){
                perror("sigqueue error");
                exit(EXIT_FAILURE);
            }
                
        }

        /* usleep(int micro seconds)
        rate -> millisecond -> 1 millisecond = 1000 micro seconds
        usleep(rate*1000)
        */
        usleep(rate*1000);
    }

    // close consumer process
    for (int j = 0; j < consumer; j++){
        if (kill(pid_list[j], SIGINT) == -1){
            perror("send SIGINT error");
            exit(EXIT_FAILURE);
        }
    }

    for (int j = 0; j < consumer; j++){
        waitpid(pid_list[j], NULL, 0);
    }

        

    // print result
    printf("\n");
    double loss = 1.0 - ((double)addr->received_num/(double)(data_num*consumer));
    printf("M=%d  R=%dms  N=%d\n", data_num, rate, consumer);
    printf("---------------------------------\n");
    printf("Total messages: %d\n", data_num*consumer);
    printf("Sum of received messages by all consumers: %d\n", addr->received_num);
    printf("Loss rate: %f\n", loss);
    printf("---------------------------------\n");

    // close shared memory
    if (shm_unlink(SHM_FILE) == -1){
        perror("shm unlink error");
        exit(EXIT_FAILURE);
    }


    exit(EXIT_SUCCESS);
}