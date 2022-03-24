/* Petros Apostolou | ID: apost035 */
/* POSIX Threads Assignment - Part 2 */
/* Program: modifier.c */


#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <error.h>
#include <string.h>
#include <sys/time.h>
#include "shmem.h"

int main(int argc, char* argv[])
{

    // parse args
    size_t N = atoi(argv[1]);


    // allocate shmem segment for Buffer 
    int Buffer = shmget(shmem_bfkey, N * sizeof(shmem_buffer), flag);
    
    // map address for Buffer
    shmem_buffer* buffer = shmat(Buffer, NULL, 0);
    if(buffer == (void*)-1)
    {
        perror("shmem segment for buffer failed");
        exit(1);
    }

    // allocate shmem segment for Semaphores
    int Semaphores = shmget(shmem_smkey, sizeof(shmem_semaphores), flag);

    // map address for semaphores
    shmem_semaphores* semaphores = shmat(Semaphores, NULL, 0);
    if(semaphores == (void*)-1)
    {
        perror("smem segment for semaphores failed");
        exit(1);
    }

    struct timeval TIME;
    char TIME_NEW[64];
    int modf_idx = 0;

    // while "EOS" is not here SPIN
    while(1)
    {
        // only 1 item modified by modifier at a time
        sem_wait(&semaphores->sem_modf);

        // if "EOS" ishere signal consumer and break
        if(strcmp(buffer[modf_idx].msg, "EOS") == 0)
        {   
            sem_post(&semaphores->sem_cons);
            break;
        }

        // let's get the timestamp
        gettimeofday(&TIME, NULL);

        sprintf(TIME_NEW, " %ld-%ld", TIME.tv_sec, TIME.tv_usec);

        // append new time to the buffer's msg part
        strcat(buffer[modf_idx].msg, TIME_NEW);

        // update circular buffer index
        modf_idx = (modf_idx + 1) % N;

        // send to consumer
        sem_post(&semaphores->sem_cons);
    }

    // deallocate shmem segments
    shmdt( (void *) buffer); shmdt( (void *) semaphores);

    return EXIT_SUCCESS;
}
