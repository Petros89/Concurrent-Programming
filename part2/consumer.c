/* Petros Apostolou | ID: apost035 */
/* POSIX Threads Assignment - Part 2 */
/* Program: consumer.c */

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

    size_t N = atoi(argv[1]);

    // allocate shmem segment for Buffer
    int Buffer = shmget(shmem_bfkey, N * sizeof(shmem_buffer), flag);
    
    // map shmem address for Buffer
    shmem_buffer* buffer = shmat(Buffer, (void*) NULL, 0);
    if(buffer == (void*)-1)
    {
        perror("map shmem segment for buffer failed");
        exit(1);
    }

    // allocate shmem segment for Semaphores
    int Semaphores = shmget(shmem_smkey, sizeof(shmem_semaphores), flag);

    // map shmem segment for Semaphores
    shmem_semaphores* semaphores = shmat(Semaphores, (void*) NULL, flag);
    if(semaphores == (void*)-1)
    {
        perror("map shmem segment for Semaphores failed");
        exit(1);
    }

    FILE* cons_stdout = fopen("consumer.log", "w");

    int cons_idx = 0;

    // while "EOS" is not here SPIN
    while(1)
    {
        // write 1 item at a time
        sem_wait(&semaphores->sem_cons);

        // if you see "EOS" break and terminate process
        if(strcmp(buffer[cons_idx].msg, "EOS") == 0)
        {
            break;
        }

        // write out to file
        fprintf(cons_stdout, "%d %s\n", buffer[cons_idx].seq, buffer[cons_idx].msg);
        printf("%d %s\n", buffer[cons_idx].seq, buffer[cons_idx].msg);

        // update next item's index in curcular buffer up to buffer size
        cons_idx = (cons_idx + 1) % N;

        // signal producer to continue buffer loading
        sem_post(&semaphores->sem_prod);
    }

    // deallocate shmem segments
    shmdt( (void *) buffer); shmdt( (void *) semaphores);

    // close output file
    fclose(cons_stdout);

    return EXIT_SUCCESS;
}
