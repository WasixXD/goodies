#define PICKY_IMPLEMENTATION
#include "picky.h"

#define TICKY_IMPLEMENTATION
#include "ticky.h"

#include <unistd.h>

// Simple test functions to benchmark
void fast_function() {
    volatile int x = 0;
    for (int i = 0; i < 10; i++) {
        x += i;
    }
}

void medium_function() {
    volatile int x = 0;
    for (int i = 0; i < 1000; i++) {
        x += i;
    }
}

void slow_function() {
    volatile int x = 0;
    for (int i = 0; i < 100000; i++) {
        x += i;
    }
}

void empty_function() {
    // Does nothing - measures overhead
}

void test_stats_creation(T *t) {
    picky_test(t, "can create new stats object");
    ticky_stats *stats = ticky_new_stats();
    picky_assertNotNull(t, stats);
    
    picky_test(t, "stats object initializes with zero results");
    picky_int_toBe(t, 0, stats->n_results);
    
    picky_test(t, "stats results array is allocated");
    picky_assertNotNull(t, stats->results);
    
    free(stats->results);
    free(stats);
}

void test_single_benchmark(T *t) {
    printf("\n  Running single benchmark:\n");
    
    picky_test(t, "can benchmark a simple function");
    ticky_stats *stats = ticky_new_stats();
    ticky_bench(stats, "Fast Function", fast_function, NULL);
    
    picky_test(t, "benchmark adds result to stats");
    picky_int_toBe(t, 1, stats->n_results);
    
    picky_test(t, "result has positive iterations");
    picky_assert(t, stats->results[0]->iterations > 0);
    
    picky_test(t, "result has positive average time");
    picky_assert(t, stats->results[0]->avg > 0);
    
    picky_test(t, "result name matches input");
    picky_assert(t, stats->results[0]->name != NULL);
    
    ticky_plot(stats);
}

void test_multiple_benchmarks(T *t) {
    printf("\n  Running multiple benchmarks:\n");
    
    picky_test(t, "can benchmark multiple functions");
    ticky_stats *stats = ticky_new_stats();
    
    ticky_bench(stats, "Empty Function", empty_function, NULL);
    ticky_bench(stats, "Fast Function", fast_function, NULL);
    ticky_bench(stats, "Medium Function", medium_function, NULL);
    
    picky_test(t, "all benchmarks are recorded");
    picky_int_toBe(t, 3, stats->n_results);
    
    picky_test(t, "all results have valid data");
    picky_assert(t, stats->results[0]->iterations > 0);
    picky_assert(t, stats->results[1]->iterations > 0);
    picky_assert(t, stats->results[2]->iterations > 0);
    
    ticky_plot(stats);
}

void test_performance_ordering(T *t) {
    printf("\n  Testing performance ordering:\n");
    
    ticky_stats *stats = ticky_new_stats();
    
    ticky_bench(stats, "Fast (10 iters)", fast_function, NULL);
    ticky_bench(stats, "Medium (1k iters)", medium_function, NULL);
    ticky_bench(stats, "Slow (100k iters)", slow_function, NULL);
    
    picky_test(t, "fast function has more iterations than medium");
    picky_assert(t, stats->results[0]->iterations > stats->results[1]->iterations);
    
    picky_test(t, "medium function has more iterations than slow");
    picky_assert(t, stats->results[1]->iterations > stats->results[2]->iterations);
    
    picky_test(t, "fast function has lower average time than slow");
    picky_assert(t, stats->results[0]->avg < stats->results[2]->avg);
    
    ticky_plot(stats);
}

void test_empty_function_benchmark(T *t) {
    printf("\n  Benchmarking minimal overhead:\n");
    
    picky_test(t, "can benchmark empty function");
    ticky_stats *stats = ticky_new_stats();
    ticky_bench(stats, "Empty Function (overhead)", empty_function, NULL);
    
    picky_test(t, "empty function has very high iteration count");
    picky_assert(t, stats->results[0]->iterations > 1000000);
    
    picky_test(t, "empty function has very low average time");
    picky_assert(t, stats->results[0]->avg < 0.000001);
    
    ticky_plot(stats);
}

void test_null_opts_handling(T *t) {
    printf("\n  Testing NULL options:\n");
    
    picky_test(t, "handles NULL opts parameter");
    ticky_stats *stats = ticky_new_stats();
    ticky_bench(stats, "Function with NULL opts", fast_function, NULL);
    
    picky_test(t, "benchmark succeeds with NULL opts");
    picky_int_toBe(t, 1, stats->n_results);
    
    ticky_plot(stats);
}

void test_result_data_integrity(T *t) {
    printf("\n  Testing result data integrity:\n");
    
    ticky_stats *stats = ticky_new_stats();
    ticky_bench(stats, "Test Function", fast_function, NULL);
    
    picky_test(t, "result name is preserved");
    picky_assertNotNull(t, stats->results[0]->name);
    
    picky_test(t, "iterations and average are consistent");
    double calculated_iters = 1.0 / stats->results[0]->avg;
    double tolerance = calculated_iters * 0.1; // 10% tolerance
    picky_assert(t, 
        stats->results[0]->iterations >= (calculated_iters - tolerance) &&
        stats->results[0]->iterations <= (calculated_iters + tolerance)
    );
    
    ticky_plot(stats);
}

int main(int argc, char **argv) {
    printf("=== TICKY TESTS ===\n\n");
    printf("Note: Each benchmark runs for ~2.5 seconds\n\n");
    
    picky_describe("Stats Creation", test_stats_creation);
    picky_describe("Single Benchmark", test_single_benchmark);
    picky_describe("Multiple Benchmarks", test_multiple_benchmarks);
    picky_describe("Performance Ordering", test_performance_ordering);
    picky_describe("Empty Function Benchmark", test_empty_function_benchmark);
    picky_describe("NULL Options Handling", test_null_opts_handling);
    picky_describe("Result Data Integrity", test_result_data_integrity);
    
    return 0;
}