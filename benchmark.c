#include "benchmark.h"

#include "rbtree.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

// Flags
int num_threads = 2;
bool correctness = false;
int num_repetitions = 3;
bool synced = false;

int main(int argc, char* argv[]) {
    
    // Parse arguments
    char c;
    while((c = getopt(argc, argv, "crt")) != -1) {
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

    test_t* tests = malloc(test_count * sizeof(test_t));
    int num_tests = 0;
    for(int i = optind; i < argc; i++) {
        // Skip invalidated inputs
        if(argv[i] == NULL) continue;

        if(parse_test(argv[i], &tests[num_tests]) < 0) {
            fprintf(stderr, "Error parsing test %s\n", argv[i]);
            continue;
        }

        num_tests++;
    }

    // Benchmark each test
    for(int i = 0; i < num_tests; i++) {
        printf("Benchmarking test [%s]\n", tests[i].filename);
        // Record times of each implementation as (insert avg, insert stdev,
        // remove avg, remove stdev, lookup avg, lookup stdev)
        double times[NUM_RB_IMPL * 6];
        for(int impl = 0; impl < NUM_RB_IMPL; impl++) {
            benchmark(impl, &tests[i], &times[impl * 6]);
        }

        // Print results
        // TODO line
        printf("Implementation\tAvg insert\tWorst insert\tAvg remove\tWorst remove\tAvg lookup\tWorst lookup\n");
        for(int impl = 0; impl < NUM_RB_IMPL; impl++) {
            printf("%s\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n", rb_names[impl],
                    times[impl * 6],
                    times[impl * 6 + 1],
                    times[impl * 6 + 2],
                    times[impl * 6 + 3],
                    times[impl * 6 + 4],
                    times[impl * 6 + 5]);
        }
    }

    // Free tests
    for(int i = 0; i < num_tests; i++) {
        free(tests[i].ops);
    }
    free(tests);

    return 0;
}

int parse_test(char* filename, test_t* test) {
    FILE* fp = fopen(filename, "r");
    if(fp == NULL) {
        return -1;
    }

    ssize_t read;
    char* line = NULL;
    size_t len = 0;

    // Fetch the number of ops
    if((read = getline(&line, &len, fp)) != -1) {
        return -1;
    }
    test->lines = (int) strtol(line, NULL, 10);
    if(test->lines < 1 || errno == EINVAL) {
        return -1;
    }

    test->ops = malloc(test->lines * sizeof(op_t));

    for(int i = 0; i < test->lines; i++) {
        // Abort on read error
        if((read = getline(&line, &len, fp)) == -1) {
            free(test->ops);
            return -1;
        }

        // Scan into op
        sscanf(line, "%d %d %d-%d", &test->ops[i].type, &test->ops[i].key,
                &test->ops[i].min_time, &test->ops[i].max_time);

        // Abort on invalid data
        if(test->ops[i].type < 0 || test->ops[i].type > 2) {
            free(test->ops);
            return -1;
        }
    }

    // Update filename
    test->filename = filename;

    fclose(fp);

    return 0;
}

void benchmark(int implementation, test_t* test, double* times) {
    // Initialize pthread barrier
    pthread_barrier_t barrier;
    if(pthread_barrier_init(&barrier, NULL, num_threads)) {
        //TODO
    }

    pthread_t pthreads[num_threads];
    double thread_times[num_threads * 3];

    size_t rbtree_size;
    switch(implementation) {
        case 0:
            //TODO
            //rbtree_size = sizeof();
            break;
        default:
            rbtree_size = 0;
            break;
    }
    void* rbtree = malloc(rbtree_size);
    //TODO errors
    rb_inits[implementation](rbtree);

    // Create arguments and launch threads
    benchmark_args_t args[num_threads];
    for(int i = 0; i < num_threads; i++) {
        args[i].implementation = implementation;
        args[i].test = test;
        args[i].times = &thread_times[i * 3];
        args[i].barrier = &barrier;
        args[i].rbtree = rbtree;
        pthread_create(&pthreads[i], NULL, benchmark_thread, &args[i]);
    }

    // Wait for threads to finish
    for(int i = 0; i < num_threads; i++) {
        pthread_join(pthreads[i], NULL);
    }

    rb_destroys[implementation](rbtree);

    // Calculate stats
    for(int j = 0; j < 3; j++) {
        // Average
        times[j * 2] = 0;
        // Maximum
        times[j * 2 + 1] = thread_times[j];

        for(int i = 0; i < num_threads; i++) {
            times[j * 2] += thread_times[i * 3 + j];
            times[j * 2 + 1] = times[j * 2 + 1] < thread_times[i * 3 + j] ?
                thread_times[i * 3 + j] : times[j * 2 + 1];
        }

        times[j * 2] /= num_threads;
    }
}

void* benchmark_thread(void* args) {
    int implementation = ((benchmark_args_t*) args)->implementation;
    test_t* test = ((benchmark_args_t*) args)->test;
    double* times = ((benchmark_args_t*) args)->times;
    pthread_barrier_t* barrier = ((benchmark_args_t*) args)->barrier;
    void* rbtree = ((benchmark_args_t*) args)->rbtree;

    // Data to calculate average
    int num[3] = {0, 0, 0};
    double avg[3] = {0, 0, 0};

    //TODO errors
    pthread_barrier_wait(barrier);

    for(int i = 0; i < test->lines; i++) {
        op_t op = test->ops[i];

        clock_t start = clock();

        switch(op.type) {
            case 0:
                rb_inserts[implementation](rbtree, op.key, 0);
                break;
            case 1:
                rb_removes[implementation](rbtree, op.key);
                break;
            case 2:
                rb_lookups[implementation](rbtree, op.key);
                break;
        }
        
        clock_t end = clock();
        // Time in milliseconds
        double time = ((double) (end - start)) / (CLOCKS_PER_SEC / 1000);
        num[op.type]++;
        avg[op.type] += (time - avg[op.type]) / (num[op.type]);

        // Sleep for a random amount of time
        struct timespec ts;
        int res;
        int sleep_time = rand() % (op.max_time - op.min_time) + op.min_time;
        ts.tv_sec = sleep_time / 1000;
        ts.tv_nsec = (sleep_time % 1000) * 1000000;
        do {
            res = nanosleep(&ts, &ts);
        } while(res && errno == EINTR);
    }

    // Record average
    for(int i = 0; i < 3; i++) {
        times[i] = avg[i];
    }

    return NULL;
}
