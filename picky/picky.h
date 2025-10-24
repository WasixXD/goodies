/******************************************************************************
 * PICKY - Lightweight Testing Library for C
 * 
 * FUNCTIONS:
 * 
 * picky_describe(message, func)
 *     Creates a test suite with a descriptive message.
 *     Groups related tests together under a common label.
 *     Parameters:
 *       - message: description of the test suite
 *       - func: function containing the tests (must accept T* parameter)
 * 
 * picky_test(t, message)
 *     Starts a new test case with a description.
 *     Must be followed by an assertion macro.
 *     Parameters:
 *       - t: test context pointer
 *       - message: description of what's being tested
 * 
 * picky_assert(t, condition)
 *     Asserts that a condition is true.
 *     Prints PASS if true, FAIL with details if false.
 * 
 * picky_assertNotNull(t, var)
 *     Asserts that a variable is not NULL.
 *     Shorthand for picky_assert(t, var != NULL).
 * 
 * picky_int_toBe(t, expected, actual)
 *     Asserts that two integers are equal.
 *     Shows expected and actual values on failure.
 * 
 * picky_float_toBe(t, expected, actual)
 *     Asserts that two floats are equal.
 *     Shows expected and actual values on failure.
 * 
 * picky_deepEqual(t, struct1, struct2, cmp)
 *     Asserts that two structs are equal using a custom comparison function.
 *     Parameters:
 *       - t: test context pointer
 *       - struct1, struct2: structs to compare
 *       - cmp: comparison function returning bool (true if equal)
 * 
 * USAGE:
 * In one C file:
 *     #define PICKY_IMPLEMENTATION
 *     #include "picky.h"
 * 
 * In other files:
 *     #include "picky.h"
 * 
 * EXAMPLES:
 *     void test_arithmetic(T *t) {
 *         picky_test(t, "addition works correctly");
 *         picky_int_toBe(t, 4, 2 + 2);
 *         
 *         picky_test(t, "multiplication works correctly");
 *         picky_int_toBe(t, 6, 2 * 3);
 *     }
 *     
 *     void test_pointers(T *t) {
 *         int *ptr = malloc(sizeof(int));
 *         picky_test(t, "malloc returns non-null pointer");
 *         picky_assertNotNull(t, ptr);
 *         free(ptr);
 *     }
 *     
 *     int main(int argc, char **argv) {
 *         picky_describe("Arithmetic Tests", test_arithmetic);
 *         picky_describe("Pointer Tests", test_pointers);
 *         return 0;
 *     }
 * 
 * OUTPUT FORMAT:
 *     [Arithmetic Tests]
 *         addition works correctly ... PASS
 *         multiplication works correctly ... PASS
 *     [Pointer Tests]
 *         malloc returns non-null pointer ... PASS
 *     === PICKY TEST END ===
 *     Tests Total = 3
 *     Tests Passed = 3
 *     Tests Failed = 0
 * 
 * NOTES:
 *     - The main() function is automatically wrapped to initialize test context
 *     - Test results are automatically printed at program end
 *     - Failed tests show file, line number, and failure reason
 * 
 * LICENSE:
 * MIT
 * VERSION: 1.0.0
 * AUTHOR: Wasix 
 *****************************************************************************/
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