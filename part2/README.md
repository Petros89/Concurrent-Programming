# Concurrent Programming Using Shared Memory & Process Synchronization

## Description
- This directory includes the Part 2 of the producer-modifier-consumer problem using shared
memory segment and POSIX Threads primitives for their synchronization. In agreement with
the description of the Assign 3 - Part 2 we create 3 Linux Processes (producer, modifier
& consumer). The producer handles producing items - in this exercise a circular buffer
stream with timestamps and their index sequences and writes whatever is produced at an
output log file --> producer.log. After all items are produced an extra item --> "EOS"
is produced to signal the END of the buffering operstion and the termination of the thread
producer. The modifier handles the modification of the buffer message - in this exercise
appending a new timestamp separated with single space in the initial buffer stream and
sends it to the consumer to print it out. The consumer receives the modified items and
prints them by "first-comes-first-isprinted" - like order to an output log
file --> consumer.log until it sees the "EOS" string where immediately terminates.

## Process synchronization
- The synchronization and the shared memory processes is implemented using POSIX Thread
primitives --> counting semaphores. There are two operations on semaphores:
"sem_wait" for blocking a semaphore when producing, modifying or consuming
that guarantess mutual exclusion and avoidance of race conditions and
"sem_post" for safely signaling (unblocking) another process to continue the same
sequencial opearation in the next item to be produced-modified-consumed etc...
The shared memory segments are created for 2 shared structures: The Buffer and
the Semaphores and are both located inside the header file "shmem.h".
For the shmem keys 2 random positive integers are used and for their flag the
number 1023 = 111111111 in binary is used taken from the Shared Memory Examples
in the parent.c program. For the size we dynamically allocate using shmget the required
size of the circular buffer - in this exercise buffer_size = N.

## Files and examples used
- The pointerCast.c file used to learn how to dynamically allocate buffer struct
  containing its sequence and its message as in the part 1.
- Also the prodcons.c file in the examples is used for certain operations and
  for understanding the flow of the blocking and unblocking of the 3 threads
  that helped a lot to understand where to use sem_wait and where sem_post on
  the counting semaphores implementation.
- The sharedMemory examples incuding both parent.c and child.c are used and
  read carefully to understand how to allocate <shmget>, map <shmat>, detach
  <shmdt> and delete <shmctl> the shared memory segments for 2 structures:
  shared memory buffers and shared memory semaphores located in the shmem.h header file.
  Here it is worth mentioning that the current flag sets all modes and permissions
  true but for different programs it may not be appropriate flag. At the parent.c
  example it states that the Least significant 9 bits indicate rwx permission mode as
  in file system. After this it states that The 10th bit IPC_CREAT is true, indicating
  that a new segment is to created. In this assignment the IPC_CREAT flag value is not used
  and experimented but it may be very useful for different shared memory applications.  
- A few online examples from the source below also helped the understanding for
  reading and writing shared memory processes in Linux. The online source link is attached
  below:
  
  Turorialspoint --> Process Cumminication --> Shared Memory:
  https://www.tutorialspoint.com/inter_process_communication/inter_process_communication_shared_memory.htm


## Compiling/Running Instructions
- To compile type: make
- To execute type: ./prodcons <buffer.size> <total_items (default == 1000)>
- To run with non-default optiona: ./prodcons 3 6000
- To clean objects type: make clean
- To delete log files type: make rmlog
