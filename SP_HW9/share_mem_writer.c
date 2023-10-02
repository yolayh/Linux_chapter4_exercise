/* usage: 
            ./writer < /etc/services &
            ./reader > out.txt

*/


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
    int cnt;                    /* Number of bytes used in 'buf' */
    char buf[4096];         /* Data being transferred */
};

//////// semphore ////////////////////////////////////

union semun {                   /* Used in calls to semctl() */
    int                 val;
    struct semid_ds *   buf;
    unsigned short *    array;
#if defined(__linux__)
    struct seminfo *    __buf;
#endif
};
 
// Boolean bsUseSemUndo = FALSE;
// Boolean bsRetryOnEintr = TRUE;
 
int                     /* Initialize semaphore to 1 (i.e., "available") */
initSemAvailable(int semId, int semNum)
{
    union semun arg;
 
    arg.val = 1;
    return semctl(semId, semNum, SETVAL, arg);
}
 
int                     /* Initialize semaphore to 0 (i.e., "in use") */
initSemInUse(int semId, int semNum)
{
    union semun arg;
 
    arg.val = 0;
    return semctl(semId, semNum, SETVAL, arg);
}
 
 /* Reserve semaphore (blocking), return 0 on success, or -1 with 'errno'
    set to EINTR if operation was interrupted by a signal handler */
 
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


//////// MAIN ////////////////////////////////////


int main (int argc, char*argv[]){

    int fd;
    int bytes, xfrs;
    struct shm *shmp;
    int semid;
    union semun dummy;
    // size_t shm_size = sizeof(struct shm);
    size_t readn;
    key_t semakey;

    
    // create shared_memory
    fd = shm_open(SHM_FILE, O_RDWR | O_CREAT, 0777);
    if (fd == -1){
        perror("shm_open error");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, 4096) == -1){
        perror("ftruncate error");
        exit(EXIT_FAILURE);
    }

    shmp = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (close(fd) == -1){
        perror("close fd error");
        exit(EXIT_FAILURE);
    }

    // create semaphore
    semid = semget(SEM_KEY, 2, IPC_CREAT | perms);
    if (semid == -1){
        perror("semget error");
        exit(EXIT_FAILURE);
    }
 
    if (initSemAvailable(semid, WRITE_SEM) == -1){
        perror("initSemAvailable error");
        exit(EXIT_FAILURE);
    }

    if (initSemInUse(semid, READ_SEM) == -1){
        perror("initSemInUse error");
        exit(EXIT_FAILURE);
    }


    for (xfrs = 0, bytes = 0; ; xfrs++, bytes += shmp->cnt){

        // check semaphore
        if (reserveSem(semid, WRITE_SEM) == -1) {
            perror("reserveSem WRITE_SEM error");
            exit(EXIT_FAILURE);
        }

        // read from stdin
        shmp->cnt = read(STDIN_FILENO, shmp->buf, 4096);
        if (shmp->cnt == -1){
            perror("read error");
            exit(EXIT_FAILURE);
        }

        // semaphore +1 let reader read
        if (releaseSem(semid, READ_SEM) == -1){
            perror("releaseSem READ_SEM error");
            exit(EXIT_FAILURE);
        }

        // check read end
        if (shmp->cnt == 0){
            break;
        }
        
    }


    // check read finish 
    if (reserveSem(semid, WRITE_SEM) == -1){
        perror("reserveSem error me??");
        exit(EXIT_FAILURE);
    } 

    // delete semaphore
    if (semctl(semid, 0, IPC_RMID, dummy) == -1){
        perror("semctl unlink error");
        exit(EXIT_FAILURE);
    }


    // close shared memory
    if (shm_unlink(SHM_FILE) == -1){
        perror("shm unlink error");
        exit(EXIT_FAILURE);
    }

    printf("sent %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}