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

            ops(lines);

            // Parse each line
            for(int i = 0; i < lines; i++) {
                file >> ops[i].type >> ops[i].key >> ops[i].min_time >>
                    ops[i].max_time;
            }
        }
};

// TODO template?
void benchmark(RBTree<int>& tree, const Test& test, double* times);

void benchmark_thread(RBTree<int>& tree, const Test& test, double* times);

#endif // BENCHMARK_H
