/* Petros Apostolou | ID: apost035 */
/* POSIX Threads Assignment - Part 2 */
/* Header: shmem.h (contains all shmem segment structures) */

#ifndef SHMEM_H
#define SHMEM_H

#include <semaphore.h>

// keys for shared processes
key_t shmem_bfkey = 1989;
key_t shmem_smkey = 2989;

// flag for shared memory segment as shown in parent.c sharedmemory examples
int flag = 1023;

// shared buffer struct
typedef struct shmem_buffer
{
    char msg[64];
    int seq;
} shmem_buffer;

// shared semaphore struct
typedef struct shmem_semaphores
{
    sem_t sem_prod;
    sem_t sem_modf;
    sem_t sem_cons;
} shmem_semaphores;

#endif
