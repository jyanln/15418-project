#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

typedef struct {
    // 0 for insert, 1 for remove, 2 for lookup
    int type;

    int key;

    // Time (in milliseconds) to wait after the operation (simulates work)
    // Time is randomly generated between the min and max
    int min_time;
    int max_time;
} op_t;

typedef struct {
    char* filename;
    int lines;
    op_t* ops;
} test_t;

typedef struct {
    int implementation;
    test_t* test;
    double* times;
    pthread_barrier_t* barrier;
    void* rbtree;
} benchmark_args_t;

// Parses a test from a file
int parse_test(char* filename, test_t* test);

// TODO benchmarks operations and record the times
void benchmark(int implementation, test_t* test, double* times);

void* benchmark_thread(void* args);

#endif // BENCHMARK_H
