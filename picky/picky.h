#ifndef PICKY_H


#ifdef PICKY_IMPLEMENTATION
#include <stdbool.h>
#include <stdio.h>
#include <malloc.h>

typedef struct {
    int tests_total;
    int test_passed;
    int test_failed;

} T;

typedef void (* test_func)(T*);
typedef bool (* cmp_function)(void *, void*);

T *t_main = NULL;

#define picky_describe(message, func) \
    do { \
        printf("[%s]\n", message); \
        func(t_main); \
    } while(0)

#define picky_test(t, message) \
    do { \
        printf("\t%s ...", message);\
        (t)->tests_total++; \
    } while(0)

#define picky_assert(t, condition) \
    do { \
        if((condition)) { \
            (t)->test_passed++; \
            printf(" PASS\n"); \
        } else { \
            (t)->test_failed++; \
            picky__fail("assert failed", __FILE__, __LINE__); \
        } \
    } while(0)

#define picky_assertNotNull(t, var) \
    do { \
        picky_assert((t), (var) != NULL); \
    } while(0)

#define picky_deepEqual(t, _struct, _struct2, cmp) \
    do { \
        if((cmp((typeof(_struct))_struct, (typeof(_struct2))_struct2))) { \
            (t)->test_passed++; \
            printf(" PASS\n"); \
        } else { \
            (t)->test_failed++; \
            char buf[128]; \
            snprintf(buf, sizeof(buf), "%s not deeply equal to %s", #_struct, #_struct2); \
            picky_fail(buf, __FILE__, __LINE__); \
        } \
    } while(0)


#define picky__fail(motive, filename, line) \
    do { \
        printf("\t FAILED\n"); \
        printf("\t.\n"); \
        printf("\t. %s:%d %s\n", filename, line, motive); \
        printf("\t.\n"); \
    } while(0)

#define picky_int_toBe(t, expected, actual) \
    do { \
        if((expected) == (actual)) { \
            (t)->test_passed++; \
            printf(" PASS\n"); \
        } else { \
            (t)->test_failed++; \
            char buf[128]; \
            snprintf(buf, sizeof(buf), "expected %d got %d", expected, actual); \
            picky__fail(buf, __FILE__, __LINE__); \
        } \
    } while(0)


#define picky_float_toBe(t, expected, actual) \
    do { \
        if((expected) == (actual)) { \
            (t)->test_passed++; \
            printf(" PASS\n"); \
        } else { \
            (t)->test_failed++; \
            char buf[128]; \
            snprintf(buf, sizeof(buf), "expected %f got %f", expected, actual); \
            picky__fail(buf, __FILE__, __LINE__); \
        } \
    } while(0)


static T *picky__newT() {
    T *test = (T*)malloc(sizeof(T));
    return test;
}

void picky_end(T *t) {
    printf("=== PICKY TEST END ===\n");
    printf("Tests Total = %d\n", t->tests_total);
    printf("Tests Passed = %d\n", t->test_passed);
    printf("Tests Failed = %d\n", t->test_failed);
}


#define main(...) \
    user_main(__VA_ARGS__); \
    int main(int argc, char **argv) { \
        t_main = picky__newT(); \
        int result; \
        result = user_main(argc, argv); \
        picky_end(t_main); \
        free(t_main); \
        return result; \
    } \
    int user_main(__VA_ARGS__)

#endif // PICKY_IMPLEMNTATION
#endif // PICKY_H