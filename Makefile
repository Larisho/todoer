CC=clang
CFLAGS=-Wall -pedantic -O2

todoer: todoer.o queue.o
	$(CC) -o todoer todoer.o queue.o
