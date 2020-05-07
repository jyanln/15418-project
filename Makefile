CC=g++

DEBUG=0
CFLAGS=-g -O3 -Wall -DDEBUG=$(DEBUG) -std=c++11 -lpthread
LDFLAGS= -lm

CPPFILES = mutex_rbtree.cpp cas_rbtree.cpp shared_rbtree.cpp benchmark.cpp
HFILES = benchmark.hpp rbtree.hpp

all: benchmark

benchmark: $(CPPFILES) $(HFILES)
	$(CC) $(CFLAGS) -o benchmark $(CPPFILES) $(LDFLAGS)

clean:
	rm -f benchmark
