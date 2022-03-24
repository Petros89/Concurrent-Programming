/* Petros Apostolou | ID: apost035 */
/* POSIX Threads Assignment - Part 2 */
/* Program: producer.c */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <error.h>
#include <string.h>
#include <sys/time.h>
#include "shmem.h"

int main(int argc, char* argv[])
{

    // pass the buffe size value 
    size_t N = atoi(argv[1]);
    
    // pass the maximum number of items value
    size_t NUM = atoi(argv[2]);

    
    // create shmem segment for Buffer struct 
    int Buffer = shmget(shmem_bfkey, N * sizeof(shmem_buffer), flag);
    
    // map shmem buffer address
    shmem_buffer* buffer = shmat(Buffer, (void*) NULL, flag);
    if(buffer == (void*)-1)
    {
        perror("shared mem segment for buffer failed");
        exit(1);
    }

    // create shmem segment for Semaphores struct 
    int Semaphores = shmget(shmem_smkey, sizeof(shmem_semaphores), flag);

    // map shmem semaphores address
    shmem_semaphores* semaphores = shmat(Semaphores, (void*) NULL, flag);
    if(semaphores == (void*)-1)
    {
        perror("address to be mapped for semaphores failed");
        exit(1);
    }

    struct timeval TIME;
    int prod_idx = 0;
    
    // write data to output file
    FILE* prod_stdout = fopen("producer.log", "w");

     
    for(int i = 0; i < NUM; i++)
    {
        // block producer process - only 1 read at a time
        sem_wait(&semaphores->sem_prod);

        // let's get timestamp
        gettimeofday(&TIME, NULL);

        // copy data on buffer
        buffer[prod_idx].seq = i + 1;
        sprintf(buffer[prod_idx].msg, "%ld-%ld", TIME.tv_sec, TIME.tv_usec);

        // print data to output file
        fprintf(prod_stdout, "%d %s\n", buffer[prod_idx].seq, buffer[prod_idx].msg);

        printf("%d %s\n", buffer[prod_idx].seq, buffer[prod_idx].msg);

        // increment circular buffer by 1 up to buffer size N
        prod_idx = (prod_idx + 1) % N;

        // unblock modifier process 
        sem_post(&semaphores->sem_modf);
    }

    // block producer to load "EOS"
    sem_wait(&semaphores->sem_prod);
    sprintf(buffer[prod_idx].msg, "EOS");

    // send "EOS" to modifier
    sem_post(&semaphores->sem_modf);


    // deallocate shmem segments
    shmdt( (void *) buffer); shmdt( (void *) semaphores);

    // close output file
    fclose(prod_stdout);

    return EXIT_SUCCESS;
}
