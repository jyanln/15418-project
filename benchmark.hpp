#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "rbtree.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <pthread.h>

struct Op {
    // 0 for insert, 1 for remove, 2 for lookup
    int type;

    int key;

    // Time (in milliseconds) to wait after the operation
    // (simulates work)
    // Time is randomly generated between the min and max
    int min_time;
    int max_time;
};

class Test {
    public:
        std::string filename;
        
        std::vector<Op> ops;

        Test(std::string _filename) : filename(_filename) {
            // Open file
            std::ifstream file(filename);
            if(!file) {
                throw -1;
            }

            // Read number of lines
            int lines;
            file >> lines;

            ops.resize(lines);

            // Parse each line
            for(int i = 0; i < lines; i++) {
                file >> ops[i].type >> ops[i].key >> ops[i].min_time >>
                    ops[i].max_time;

                // Check correctness
                if(ops[i].type < 0 || ops[i].type > 2 ||
                        ops[i].max_time < ops[i].min_time) {
                    throw -2;
                }
            }
        }
};

void benchmark(RBTree* tree, const Test& test, double* times);

void benchmark_thread(const int& thread, RBTree* tree, const Test& test,
        double* times);

#endif // BENCHMARK_H
