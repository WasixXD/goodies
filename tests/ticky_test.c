#define TICKY_IMPLEMENTATION

#include <ticky.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define TICKY_TIME_RUNNING 1.0

int sum(int a, int b) {
    return a + b;
}

int fibonnaci(int n) {
    if(n <= 1) return n;
    return fibonnaci(n-1) + fibonnaci(n -2);
}

void bench_fib() {
    fibonnaci(10);
}

void bench_sum() {
    srand(time(NULL));
    int a = rand();
    int b = rand();
    sum(a, b);
}

int main() {
    ticky_stats *s = ticky_new_stats();
    ticky_bench(s, "sum()", bench_sum, NULL);
    ticky_bench(s, "fibonnachi()", bench_fib, NULL);


    ticky_plot(s);
}