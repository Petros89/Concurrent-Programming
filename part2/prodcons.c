/* Petros Apostolou | ID: apost035 */
/* POSIX Threads Assignment - Part 2 */
/* Compile: make */
/* Execute: ./prodcons <buffer.size> <total_items (default == 1000)> */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <error.h>
#include <string.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>
#include "shmem.h"



int main(int argc, char* argv[])
{
    size_t N; // buffer size
    size_t NUM = 1000; //maximum number of items (default)
    
    if(argc < 2)
    {
      perror("Usage: <./executable> <arg1> <arg2 (default == 1000)> \n");
    }

    // get buffer size
    N = atoi(argv[1]);

    // if non-default set NUM
    if(argc == 3)
    {
        NUM = atoi(argv[2]);
    }

    // buffers for execl args
    char argc1[64], argc2[64];

    // load buffer size N and numbe of items as arguments in order to parse
    // then to the child processes
    sprintf(argc1, "%lu", N);
    sprintf(argc2, "%lu", NUM);

    // initialize shmem segment for Buffer struct under flag
    int Buffer = shmget(shmem_bfkey, 0, flag);

    // delete shmem segment for Buffer struct if pre-exists
    if(Buffer != -1) //already existed segment --> delete
    {
        // delete shmem segment if any
        shmctl(Buffer, IPC_RMID, NULL);
    }

    // allocate new shmem segment for Buffer struct
    Buffer = shmget(shmem_bfkey, N * sizeof(shmem_buffer), flag);

    // map shmem buffer to the current address
    shmem_buffer* buffer = shmat(Buffer, (void*) NULL, flag);
    if(buffer == (void*)-1)
    {
        perror("shared mem segment for buffer failed");
        exit(1);
    }


    // initialize shmem segment for Semaphores struct under flag
    int Semaphores = shmget(shmem_smkey, 0, flag);

    // delelte shmem segmnt for Semaphores struct if pre-exists
    if(Semaphores != -1) //already existed segment --> delete
    {
        
        // delete shamem segment if any
        shmctl(Semaphores, IPC_RMID, NULL);
    }

    // allocate new shmem segment for Semaphores struct
    Semaphores = shmget(shmem_smkey, sizeof(shmem_semaphores), flag);

    // map shmem semaphores to the current address 
    shmem_semaphores* semaphores = shmat(Semaphores,(void *) NULL, flag);
    if(semaphores == (void*)-1)
    {
        perror("address is incorrect");
        exit(1);
    }
    

    // semaphore initialization
    sem_init(&semaphores->sem_prod, PTHREAD_PROCESS_SHARED, N);
    sem_init(&semaphores->sem_modf, PTHREAD_PROCESS_SHARED, 0);
    sem_init(&semaphores->sem_cons, PTHREAD_PROCESS_SHARED, 0);

    // detatch semaphores
    shmdt( (void*) semaphores);

    // create child process --> producer
    pid_t prod_id = fork();
    if(prod_id == 0)
    {
        execl("./producer", "producer", argc1, argc2, NULL);
    }

    // create child process --> modifier
    pid_t modf_id = fork();
    if(modf_id == 0)
    {
        execl("./modifier", "modifier", argc1, NULL);
    }

    // create child process --> cosumer
    pid_t cons_id = fork();
    if(cons_id == 0)
    {
        execl("./consumer", "consumer", argc1, NULL);
    }

    // wait for child processes to finish
    waitpid(prod_id, NULL, 0);
    waitpid(modf_id, NULL, 0);
    waitpid(cons_id, NULL, 0);

    // deallocate shmem Buffer
    shmctl(Buffer, IPC_RMID, 0); 

    // deallocate shmem Semaphores
    shmctl(Semaphores, IPC_RMID, 0); 

    return EXIT_SUCCESS;
}

