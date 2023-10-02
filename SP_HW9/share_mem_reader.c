#include <fcntl.h>             //shm
#include <sys/stat.h>          //shm
#include <sys/mman.h>          //shm
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <errno.h>

#define perms (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define SHM_FILE "/newshm_file"
#define SEM_KEY 0x1234

#define WRITE_SEM 0
#define READ_SEM 1


struct shm {                 /* Defines structure of shared memory segment */
    size_t cnt;                    /* Number of bytes used in 'buf' */
    char buf[4096];         /* Data being transferred */
};

int                     /* Reserve semaphore - decrement it by 1 */
reserveSem(int semId, int semNum)
{
    struct sembuf sops;
 
    sops.sem_num = semNum;
    sops.sem_op = -1;
    sops.sem_flg = 0;
 
    return semop(semId, &sops, 1);
}

 
int                     /* Release semaphore - increment it by 1 */
releaseSem(int semId, int semNum)
{
     struct sembuf sops;
 
     sops.sem_num = semNum;
     sops.sem_op = 1;
     sops.sem_flg = 0;
 
     return semop(semId, &sops, 1);
}

int main (int argc, char*argv[]){

    int fd;
    int xfrs, bytes;
    struct shm *shmp;
    int semid;
    key_t semakey;
    int write_n;

    

    fd = shm_open(SHM_FILE, O_RDWR, 0);
    if (fd == -1){
        perror("shm_open error");
        exit(EXIT_FAILURE);
    }

    shmp = mmap(NULL, 4096, PROT_READ, MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED){
        perror("mmap error");
        exit(EXIT_FAILURE);
    }

    if (close(fd) == -1){
        perror("close fd error");
        exit(EXIT_FAILURE);
    }

    semid = semget(SEM_KEY, 0, 0);
    if (semid == -1) {
        perror("semget error");
        exit(EXIT_FAILURE);
    }
    
    for (xfrs = 0, bytes = 0; ; xfrs++){


        // check read semaphore
        if (reserveSem(semid, READ_SEM) == -1){
            perror("reserveSem error");
            exit(EXIT_FAILURE);
        }
        
        write_n = shmp->cnt;

        if (write_n == 0){
            break;
        }
        bytes+= shmp->cnt;

        if ((write(STDOUT_FILENO, shmp->buf, write_n)) != write_n){
            perror("write stdout error");
            exit(EXIT_FAILURE);
        }

        // open write semaphore
        if (releaseSem(semid, WRITE_SEM) == -1){
            perror("releaseSem error");
            exit(EXIT_FAILURE);
        }

    }


    
    if (releaseSem(semid, WRITE_SEM) == -1) {
        perror("releaseSem error");
        exit(EXIT_FAILURE);
    }


    printf("\nreceived %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}