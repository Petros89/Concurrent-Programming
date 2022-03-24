CC=gcc
CFLAGS=-O3
PSXFLAGS=-lpthread -D_REENTRANT

MAIN=prodcons
PROD=producer
MODF=modifier
CONS=consumer


all: $(MAIN).c $(PROD).c $(MODF).c $(CONS).c
	$(CC) $(CFLAGS) $(MAIN).c $(PSXFLAGS) -o $(MAIN)
	$(CC) $(CFLAGS) $(PROD).c $(PSXFLAGS) -o $(PROD)
	$(CC) $(CFLAGS) $(MODF).c $(PSXFLAGS) -o $(MODF)
	$(CC) $(CFLAGS) $(CONS).c $(PSXFLAGS) -o $(CONS)

clean:
	rm -f $(MAIN) $(PROD) $(MODF) $(CONS)

rmlog:
	rm -f *.log
