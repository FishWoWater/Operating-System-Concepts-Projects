# makefile for thread pool
#

CC=gcc
CFLAGS=-Wall -g
PTHREADS=-lpthread

all: client.o threadpool.o
	$(CC) $(CFLAGS) -o example client.o threadpool.o $(PTHREADS)
	$(CC) $(CFLAGS) -o procon procon.c $(PTHREADS)

client.o: client.c
	$(CC) $(CFLAGS) -c client.c $(PTHREADS)

threadpool.o: threadpool.c threadpool.h
	$(CC) $(CFLAGS) -c threadpool.c $(PTHREADS)

clean:
	rm -rf *.o
	rm -rf example
	rm -rf procon

test:
	./example

procon:
	$(CC) $(CFLAGS) -o procon procon.c $(PTHREADS)
