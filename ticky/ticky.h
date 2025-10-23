/*****************************************************************************
 * TICKY - Single-Header Benchmarking Library
 * 
 * A lightweight, STB-style header-only benchmarking utility for C programs.
 * Measures function execution time and calculates iterations per second.
 *
 * USAGE:
 *   Define TICKY_IMPLEMENTATION in exactly ONE C file before including:
 *   
 *   #define TICKY_IMPLEMENTATION
 *   #include "ticky.h"
 *
 * BASIC EXAMPLE:
 *   void my_function() {
 *       // code to benchmark
 *   }
 *
 *   int main() {
 *       ticky_stats *stats = ticky_new_stats();
 *       ticky_bench(stats, "My Function", my_function, NULL);
 *       ticky_plot(stats);
 *       return 0;
 *   }
 *
 * FEATURES:
 *   - Automatic iteration counting (runs for ~2.5 seconds)
 *   - Average execution time calculation
 *   - Iterations per second measurement
 *   - Compiler detection and optimization flag reporting
 *   - Simple API with minimal overhead
 *
 * FUNCTIONS:
 *   ticky_stats* ticky_new_stats()
 *       Creates a new statistics collection for benchmark results
 *
 *   void ticky_bench(ticky_stats *s, char *message, func fn, ticky_opts *opts)
 *       Benchmarks a function and stores results
 *       - s: stats object to store results
 *       - message: description of the benchmark
 *       - fn: function pointer to benchmark (must be void (*func)())
 *       - opts: reserved for future use (pass NULL)
 *
 *   void ticky_plot(ticky_stats *s)
 *       Prints all benchmark results and frees memory
 *
 * NOTES:
 *   - Functions are executed repeatedly for approximately 2.5 seconds
 *   - Results include average time per call and iterations per second
 *   - Memory is automatically freed when calling ticky_plot()
 *   - Thread-safety is not guaranteed
 *
 * LICENSE:
 *    MIT
 *
 * VERSION: 1.0.0
 * AUTHOR: Wasic 
 *****************************************************************************/

#ifndef TICKY_H
#define TICKY_H


#ifdef TICKY_IMPLEMENTATION

#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <malloc.h>

typedef struct ticky_opts {
    char *group;
    int baseline;
} ticky_opts;

typedef struct ticky_result {
    char *name;
    int iterations;
    double avg;
} ticky_result;

typedef struct ticky_stats {
    ticky_result **results;
    int n_results;
} ticky_stats;

typedef void (*func)();


void ticky_bench(ticky_stats *s, char *message, func fn, ticky_opts *opts) {

    clock_t start_time = clock();
    size_t N = 0;
    struct timeval begin, end;

    gettimeofday(&begin, 0);

    while(((double)(clock() - start_time))/CLOCKS_PER_SEC < 2.5) {
        fn();
        N++;
    }

    gettimeofday(&end, 0);

    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds*1e-6;

    double avg = elapsed / N;
    long iters = 1 / avg;

    ticky_result *result = (ticky_result*)malloc(100 * sizeof(ticky_result));

    result->avg = avg;
    result->iterations = iters;
    result->name = message;

    s->results[s->n_results] = result;
    s->n_results++;
}


ticky_stats* ticky_new_stats() {
    ticky_stats *s = (ticky_stats*)malloc(sizeof(ticky_stats));
    s->results = (ticky_result**)malloc(100 * sizeof(ticky_result*));
    s->n_results = 0;

    return s;
}

void ticky_plot(ticky_stats *s) {
    #if defined(__clang__)
        printf("COMPILER: CLANG %d.%d.%d\n", __clang_major__, __clang_minor__, __clang_patchlevel__);
    #elif defined(__GNUC__)
        printf("COMPILER: GCC %d.%d.%d\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
    #elif defined(_MSC_VER)
        printf("COMPILER: MSVC %d\n", _MSC_VER);
    #else
        printf("COMPILER: Unknown\n");
    #endif

    #ifdef __OPTIMIZE__
        printf("OPTIMIZED: true\n");
    #else
        printf("OPTIMIZED: false\n");
    #endif

    printf("\n");
    for(size_t i = 0; i < s->n_results; i++) {
        ticky_result *t = s->results[i];
        printf("%s...AVG: %.10f | ITERS/S: %d\n", t->name, t->avg, t->iterations);
    }
    free(s);
}


#endif // TICKY_IMPLEMENTATION
#endif // TICKY_H