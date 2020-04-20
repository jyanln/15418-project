#ifndef BENCHMARK_H
#define BENCHMARK_H

enum OP_TYPE { INSERT, REMOVE, LOOKUP }

typedef struct {
    OP_TYPE type;
    int key;
    int val;
} op_t;

// TODO parses a test from a file
void parse_test(char* filename, op_t* ops);

// TODO benchmarks operations and record the times
void benchmark(int implementation, op_t* ops, double* times);

#endif // BENCHMARK_H
