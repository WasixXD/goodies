/*
 * PICKY - A Lightweight C Testing Framework
 * 
 * A minimalist single-header testing framework for C with expressive syntax.
 * Define PICKY_IMPLEMENTATION before including this header in exactly one source file.
 * 
 * Features:
 * - BDD-style test descriptions with picky_describe()
 * - Multiple assertion types (boolean, integers, floats, null checks)
 * - Deep equality comparison with custom comparator functions
 * - Automatic test counting and reporting
 * - Clean, readable output with pass/fail indicators
 * - Automatic main() function wrapper for seamless integration
 * 
 * Usage:
 *   #define PICKY_IMPLEMENTATION
 *   #include "picky.h"
 * 
 * Basic Example:
 * 
 *   #define PICKY_IMPLEMENTATION
 *   #include "picky.h"
 *   
 *   void test_basic_math(T *t) {
 *       picky_test(t, "addition should work correctly");
 *       picky_int_toBe(t, 4, 2 + 2);
 *       
 *       picky_test(t, "multiplication should work");
 *       picky_int_toBe(t, 6, 2 * 3);
 *   }
 *   
 *   void test_pointers(T *t) {
 *       int *ptr = malloc(sizeof(int));
 *       
 *       picky_test(t, "pointer should not be null");
 *       picky_assertNotNull(t, ptr);
 *       
 *       free(ptr);
 *   }
 *   
 *   int main(int argc, char **argv) {
 *       picky_describe("Basic Math Tests", test_basic_math);
 *       picky_describe("Pointer Tests", test_pointers);
 *       return 0;
 *   }
 * 
 * Advanced Example - Deep Equality:
 * 
 *   typedef struct {
 *       int x, y;
 *   } Point;
 *   
 *   bool point_equals(Point *a, Point *b) {
 *       return a->x == b->x && a->y == b->y;
 *   }
 *   
 *   void test_structures(T *t) {
 *       Point p1 = {10, 20};
 *       Point p2 = {10, 20};
 *       Point p3 = {15, 25};
 *       
 *       picky_test(t, "identical points should be equal");
 *       picky_deepEqual(t, &p1, &p2, point_equals);
 *       
 *       picky_test(t, "different points should not be equal");
 *       // This test would fail as expected
 *       // picky_deepEqual(t, &p1, &p3, point_equals);
 *   }
 *   
 *   int main(int argc, char **argv) {
 *       picky_describe("Structure Comparison", test_structures);
 *       return 0;
 *   }
 * 
 * Float Comparison Example:
 * 
 *   void test_floating_point(T *t) {
 *       float result = 0.1f + 0.2f;
 *       
 *       picky_test(t, "float addition");
 *       // Note: Direct float comparison might have precision issues
 *       picky_float_toBe(t, 0.3f, result);
 *       
 *       double pi = 3.14159;
 *       picky_test(t, "pi approximation");
 *       picky_float_toBe(t, 3.14159, pi);
 *   }
 * 
 * Boolean Assertion Example:
 * 
 *   bool is_even(int n) {
 *       return n % 2 == 0;
 *   }
 *   
 *   void test_boolean_logic(T *t) {
 *       picky_test(t, "4 should be even");
 *       picky_assert(t, is_even(4));
 *       
 *       picky_test(t, "empty string should be truthy when checked");
 *       char *str = "";
 *       picky_assert(t, str != NULL);
 *       
 *       picky_test(t, "logical AND operation");
 *       picky_assert(t, (1 && 1) == 1);
 *   }
 * 
 * Complete Test Suite Example:
 * 
 *   void test_array_operations(T *t) {
 *       int arr[] = {1, 2, 3, 4, 5};
 *       int length = sizeof(arr) / sizeof(arr[0]);
 *       
 *       picky_test(t, "array length calculation");
 *       picky_int_toBe(t, 5, length);
 *       
 *       picky_test(t, "first element access");
 *       picky_int_toBe(t, 1, arr[0]);
 *       
 *       picky_test(t, "last element access");
 *       picky_int_toBe(t, 5, arr[length - 1]);
 *   }
 *   
 *   void test_string_operations(T *t) {
 *       char *greeting = "Hello";
 *       
 *       picky_test(t, "string should not be null");
 *       picky_assertNotNull(t, greeting);
 *       
 *       picky_test(t, "string length check");
 *       picky_int_toBe(t, 5, strlen(greeting));
 *   }
 *   
 *   int main(int argc, char **argv) {
 *       printf("Running PICKY Test Suite\n\n");
 *       
 *       picky_describe("Array Operations", test_array_operations);
 *       picky_describe("String Operations", test_string_operations);
 *       picky_describe("Math Operations", test_basic_math);
 *       
 *       printf("\nTest suite completed!\n");
 *       return 0;
 *   }
 * 
 * Key Features:
 * 
 * - **picky_describe()**: Groups related tests with descriptive names
 * - **picky_test()**: Defines individual test cases with descriptions
 * - **picky_assert()**: Generic boolean assertion
 * - **picky_assertNotNull()**: Null pointer check
 * - **picky_int_toBe()**: Integer equality assertion
 * - **picky_float_toBe()**: Floating-point equality assertion  
 * - **picky_deepEqual()**: Custom struct/object comparison
 * - **picky_end()**: Automatic test summary (called by framework)
 * 
 * Output Format:
 * [Test Group Name]
 *     test description ... PASS
 *     another test ... FAILED
 *     .
 *     . filename:line error message
 *     .
 * 
 * === PICKY TEST END ===
 * Tests Total = X
 * Tests Passed = Y  
 * Tests Failed = Z
 * 
 * Notes:
 * - The framework automatically wraps your main() function
 * - Test state is managed globally through t_main
 * - Failed tests show file location and descriptive error messages
 * - All test functions should accept a T* parameter
 * - Return 0 from main() for successful test completion
 */

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