#include "benchmark.hpp"

#include "rbtree.hpp"
#include "mutex_rbtree.hpp"
#include "cas_rbtree.hpp"

#include <thread>
#include <chrono>
#include <stdio.h>
#include <errno.h>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <iostream>

// Flags
int num_threads = 2;
bool correctness = false;
int num_repetitions = 3;
bool synced = false;

int main(int argc, char* argv[]) {
    // Parse arguments
    char c;
    while((c = getopt(argc, argv, "cr:t:")) != -1) {
        switch(c) {
            case 'c':
                correctness = true;
                break;
            case 'r':
                num_repetitions = (int) strtol(optarg, NULL, 10);
                if(num_repetitions < 1 || errno == EINVAL) {
                    fprintf(stderr, "Invalid -r argument\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 's':
                synced = true;
                break;
            case 't':
                num_threads = (int) strtol(optarg, NULL, 10);
                if(num_threads < 1 || errno == EINVAL) {
                    fprintf(stderr, "Invalid -t argument\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case '?':
                if(optopt == 'r' || optopt == 't') {
                    fprintf(stderr, "Option -%c requires an argument.\n",
                            optopt);
                } else if(isprint(optopt)) {
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                } else {
                    fprintf(stderr, "Unknown option character `\\x%x'.\n",
                            optopt);
                }
           default:
                abort();
        }
    }

    if(num_threads < 1) {
        fprintf(stderr, "Invalid number of threads\n");
        exit(EXIT_FAILURE);
    }

    int test_count = 0;
    for(int i = optind; i < argc; i++) {
        struct stat path_stat;
        stat(argv[i], &path_stat);
        if(S_ISREG(path_stat.st_mode)) {
            // Regular file
            test_count++;
        } else {
            fprintf(stderr, "Provided file %s does not exist or is invalid\n",
                    argv[i]);

            // Invalidate input
            argv[i] = NULL;
        }
    }

    std::vector<Test> tests;

    int num_tests = 0;
    for(int i = optind; i < argc; i++) {
        // Skip invalidated inputs
        if(argv[i] == NULL) continue;

        try {
            tests.push_back(Test(argv[i]));
        } catch(int err) {
            fprintf(stderr, "Error parsing test %s with error [%d]\n",
                    argv[i], err);
            continue;
        }

        num_tests++;
    }

    // Benchmark each test
    for(int i = 0; i < num_tests; i++) {
        printf("Benchmarking test [%s]\n", tests[i].filename.c_str());
        double times[NUM_RB_IMPL * 6];

        RBTree* trees[NUM_RB_IMPL] = {new MutexRBTree(), new CasRBTree()};

        for(int impl = 0; impl < NUM_RB_IMPL; impl++) {
            benchmark(trees[impl], tests[i], &times[impl * 6]);
        }

        // Print results
        printf("Implementation\tAvg insert\tWorst insert\tAvg remove\tWorst remove\tAvg lookup\tWorst lookup\n");
        for(int impl = 0; impl < NUM_RB_IMPL; impl++) {
            printf("%s\t%.2f\t\t%.2f\t\t%.2f\t\t%.2f\t\t%.2f\t\t%.2f\n",
                    trees[impl]->name().c_str(),
                    times[impl * 6],
                    times[impl * 6 + 1],
                    times[impl * 6 + 2],
                    times[impl * 6 + 3],
                    times[impl * 6 + 4],
                    times[impl * 6 + 5]);
        }
    }

    return 0;
}

void benchmark(RBTree* rbtree, const Test& test, double* times) {
    std::vector<std::thread> threads;
    double min_times[6];
    double thread_times[num_threads * 3];

    for(int r = 0; r < num_repetitions; r++) {
        // Launch threads
        for(int i = 0; i < num_threads; i++) {
            threads.push_back(std::thread(benchmark_thread, i, rbtree, test,
                        &thread_times[i * 3]));
        }

        // Wait for threads to finish
        for(int i = 0; i < num_threads; i++) {
            threads[i].join();
        }
        threads.clear();

        // Calculate and store stats
        for(int j = 0; j < 3; j++) {
            // Average
            double avg = 0;
            // Maximum
            double max = thread_times[j];

            for(int i = 0; i < num_threads; i++) {
                avg += thread_times[i * 3 + j];
                max = max < thread_times[i * 3 + j] ?
                    thread_times[i * 3 + j] : max;
            }

            avg /= num_threads;

            if(r == 0 || avg < min_times[j * 2]) {
                min_times[j * 2] = avg;
                min_times[j * 2 + 1] = max;
            }
        }
    }

    // Copy min times
    for(int i = 0; i < 6; i++) {
        times[i] = min_times[i];
    }
}

void benchmark_thread(const int& thread, RBTree* tree, const Test& test,
        double* times) {
    // Data to calculate average
    int num[3] = {0, 0, 0};
    double avg[3] = {0, 0, 0};

    for(size_t i = thread; i < test.ops.size(); i += num_threads) {
        Op op = test.ops[i];

        auto start = std::chrono::steady_clock::now();

        switch(op.type) {
            case 0:
                tree->insert(op.key, 0);
                break;
            case 1:
                tree->remove(op.key);
                break;
            case 2:
                tree->lookup(op.key);
                break;
        }
        
        auto end = std::chrono::steady_clock::now();
        // Time in milliseconds
        double time =
            std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        // Update stats
        num[op.type]++;
        avg[op.type] += (time - avg[op.type]) / (num[op.type]);


        // Sleep for a random amount of time
        int sleep_time = op.max_time == op.min_time ? op.min_time :
            rand() % (op.max_time - op.min_time) + op.min_time;
        std::this_thread::sleep_for(std::chrono::microseconds(sleep_time));
    }

    // Record average
    for(int i = 0; i < 3; i++) {
        times[i] = avg[i];
    }
}
