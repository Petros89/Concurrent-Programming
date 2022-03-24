EXE=prodcons
CC=gcc
THREAD_FLAGS= -lpthread -D_REENTRANT
CFLAGS= -O3

all: $(EXE).c
	$(CC) $(CFLAGS) $(EXE).c $(THREAD_FLAGS) -o $(EXE)

clean:
	rm -f $(EXE)

rmlog:
	rm -rf *.log
