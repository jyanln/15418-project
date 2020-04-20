CC=gcc

DEBUG=0
CFLAGS=-g -O3 -Wall -DDEBUG=$(DEBUG) -std=gnu99
LDFLAGS= -lm

CFILES = benchmark.c
HFILES = benchmark.h rbtree.h

all: benchmark

benchmark: $(CFILES) $(HFILES)
	$(CC) $(CFLAGS) -o benchmark $(CFILES) $(LDFLAGS)

clean:
	rm -f benchmark
