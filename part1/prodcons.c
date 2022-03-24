/* Petros Apostolou | ID: apost035 */
/* POSIX Threads Assignment - Part 1 */
/* Compile: make */
/* Execute: ./prodcons <buffer.size> <total_items (default == 1000)> */


#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <signal.h>



/* Construct Global Attributed */
// maximum number of elements for the producer to read
int NUM = 1000; //default = 1000, otherwise specify NUM in argv2
int N; // N is the size of the buffer (how many characters can be loaded in a single thread instance)

// initialize thread instances
pthread_t prod;
pthread_t modi;
pthread_t cons;

// semaphores
sem_t sem_prod;
sem_t sem_modf;
sem_t sem_cons;

//datastruct to cast pointer Buffer and keep timestamp and its sequence number
// code instructed from example file pointerCast.c
typedef struct BufferType {
     // char to store the message that Buffer contains
     // allocate 64 single characters for timestamp usage
     char msg[64];
     // int to store the sequencial number of the message (1, 2, 3, ..., N)
     int seq;
} BufferType;
// create global casting for all threads and allocate mem in main 
BufferType* Buffer;
/**********************************************************************************/

void* Producer(void* data)
{
    struct timeval TIME;
    
    // open output file
    FILE* prod_stdout = fopen("producer.log", "w");

    // send produced items to buffer 
    int prod_idx = 0;
    for(int i = 0; i < NUM; i++)
    {
        
        // only 1 prod at a time
        sem_wait(&sem_prod);

        // let's get the sequence int
        Buffer[prod_idx].seq = i + 1;
        // let's get timestamp
        gettimeofday(&TIME, NULL);
        
        // load message into buffer
        sprintf(Buffer[prod_idx].msg, "%ld-%ld", TIME.tv_sec, TIME.tv_usec);

        // write to outfile
        fprintf(prod_stdout, "%d %s\n", Buffer[prod_idx].seq, Buffer[prod_idx].msg);
        printf("%d %s\n", Buffer[prod_idx].seq, Buffer[prod_idx].msg);

        // increment circular buffer idx for next produced item up to buffer size
        prod_idx = (prod_idx + 1) % N;

        // send to modifier
        sem_post(&sem_modf);
    }

    // only 1 producer load EOS (avoid race conditions)
    sem_wait(&sem_prod);
    // load EOS
    sprintf(Buffer[prod_idx].msg, "EOS");
    // send EOS to modifier
    sem_post(&sem_modf);

    // close output file
    fclose(prod_stdout);

    pthread_exit(NULL);
}

/* THREAD MODIFIER */
void* Modifier(void* data)
{
    struct timeval TIME;
    char new_msg[64];

    int modf_idx = 0;
    // while !EOS SPIN
    while(1)
    {
        // only 1 modifier modifies at a time (avoid race conditions)
        sem_wait(&sem_modf);

        // if EOS is received terminate
        if(strcmp(Buffer[modf_idx].msg, "EOS") == 0) {
            // send to consumer and break
            sem_post(&sem_cons);
            break;
        }

        // let's get timestamp
        gettimeofday(&TIME, NULL);

        // load new message into buffer
        sprintf(new_msg, " %ld-%ld", TIME.tv_sec, TIME.tv_usec);

        // concatenate the 2 messages (timestamps) into a single string
        strcat(Buffer[modf_idx].msg, new_msg);

        // increment circular buffer idx for next item to be modified
        modf_idx = (modf_idx + 1) % N;

        // send modified item to consumer
        sem_post(&sem_cons);
    }

    pthread_exit(NULL);
}

/* THREAD CONSUMER */
void* Consumer(void* data)
{
    // open output file
    FILE* cons_stdout = fopen("consumer.log", "w");

    
    int cons_idx = 0;
    // while !EOS SPIN
    while(1)
    {
        // only 1 writer at a time
        sem_wait(&sem_cons);

        // if EOS is here break
        if(strcmp(Buffer[cons_idx].msg, "EOS") == 0) {
            break;
        }

        // print outfile
        fprintf(cons_stdout, "%d %s\n", Buffer[cons_idx].seq, Buffer[cons_idx].msg);
        printf("%d %s\n", Buffer[cons_idx].seq, Buffer[cons_idx].msg);

        // increment circular index for next item to be written out
        cons_idx = (cons_idx + 1) % N;

        // awake next producer to load next item from the buffer
        sem_post(&sem_prod);
    }

    // close output file
    fclose(cons_stdout);

    pthread_exit(NULL);
}

/* MAIN PROCESS */
int main(int argc, char* argv[])
{
    
    if (argc < 2){
      perror("Usage: <./executable> <arg1> <arg2 (default == 1000)> \n");
    }

    N = atoi(argv[1]); // buffer size N
    if (argc == 3){
      // max items
      NUM = atoi(argv[2]);
    }

    // allocate mem to Buffer
    Buffer = malloc(N * sizeof(BufferType));

    // semaphore initialization
    sem_init(&sem_prod, 0, N);
    sem_init(&sem_modf, 0, 0);
    sem_init(&sem_cons, 0, 0);

    // create 3 threads:
    // 1 producer, 1 modifier and 1 consumer thread
    pthread_create(&prod, NULL, Producer, NULL);
    pthread_create(&modi, NULL, Modifier, NULL);
    pthread_create(&cons, NULL, Consumer, NULL);

    // wait for all threads to finish
    pthread_join(prod, NULL);
    pthread_join(modi, NULL);
    pthread_join(cons, NULL);

    // destroy semaphores
    sem_destroy(&sem_prod);
    sem_destroy(&sem_modf);
    sem_destroy(&sem_cons);
    
    return EXIT_SUCCESS;
}
