# Concurrent Programming Using POSIX Threads 

## Description
-This directory includes the Part 1 of the producer-modifier-consumer problem using POSIX
threads and counting semaphores. In agreement with the description of the Assign 3 -
Part 1 we create 3 Pthreads (producer, modifier & consumer). The producer handles
producing items - in this exercise a circular buffer stream with timestamps and their
index sequences and writes whatever is produced at an output log file --> producer.log.
After all items are produced an extra item --> "EOS" is produced to signal the END of the
buffering operstion and the termination of the thread producer. The modifier handles
the modification of the buffer message - in this exercise appending a new timestamp
separated with single space in the initial buffer stream and send it to the consumer to
print it out. The consumer receives the modified items and prints them by
"first-comes-first-isprinted" - like order to an output log file --> consumer.log until
it sees the "EOS" string where immediately terminates.

## Thread synchronization
- The synchronization and the mutual exclusions of the POSIX Threads is
implemented using counting semaphores. There are two operations on semaphores:
"sem_wait" for blocking a semaphore when producing, modifying or consuming
that guarantess mutual exclusion and avoidance of race conditions and
"sem_post" for safely signaling (unblocking) a thread to continue the same
process in the next item to be produced-modified-consumed etc...

## Files and examples used
- The pointerCast.c file used to learn how to dynamically allocate buffer struct
  containing its sequence and its message.
- Also the prodcons.c file in the examples is used for certain operations and
  for understanding the flow of the blocking and unblocking of the 3 threads
  that helped a lot to understand where to use sem_wait and where sem_post on
  the counting semaphores implementation.
- Linux man pages for sem_init, sem_wait, sem_post and sem_destroy along with a
  few online examples (see below) helped also understand how to syntax the counting
  semaphores including definition, initialization and actual wait/post
  operations:

  Oracle: https://docs.oracle.com/cd/E26502_01/html/E35303/sync-11157.html

## Compiling/Running Instructions
- To compile type: make
- To execute type: ./prodcons <buffer.size> <total_items (default == 1000)>
- To run with non-default optiona: ./prodcons 8 6000
- To clean objects type: make clean
- To delete log files type: make rmlog
