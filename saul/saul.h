/*
 * SAUL - Simple Array Utility Library
 * 
 * A lightweight single-header matrix library for basic linear algebra operations.
 * Define SAUL_IMPLEMENTATION before including this header in exactly one source file.
 * 
 * Usage:
 *   #define SAUL_IMPLEMENTATION
 *   #include "saul.h"
 * 
 * Example - Basic matrix operations:
 * 
 *   // Create two 3x3 matrices
 *   Matrix *a = saul_new_matrix(3, 3);
 *   Matrix *b = saul_new_matrix(3, 3);
 *   
 *   // Fill matrix 'a' with values
 *   saul_matrix_set_value(a, 0, 0, 1.0f);
 *   saul_matrix_set_value(a, 0, 1, 2.0f);
 *   saul_matrix_set_value(a, 1, 0, 3.0f);
 *   saul_matrix_set_value(a, 1, 1, 4.0f);
 *   
 *   // Fill matrix 'b' with values
 *   saul_matrix_set_value(b, 0, 0, 2.0f);
 *   saul_matrix_set_value(b, 0, 1, 1.0f);
 *   saul_matrix_set_value(b, 1, 0, 1.0f);
 *   saul_matrix_set_value(b, 1, 1, 2.0f);
 *   
 *   // Add matrices (result stored in 'a')
 *   if (saul_matrix_add(a, b) == 0) {
 *       printf("Addition successful\n");
 *       saul_print_matrix(a);
 *   }
 *   
 *   // Matrix multiplication
 *   Matrix *c = saul_matrix_mul(a, b);
 *   if (c != NULL) {
 *       printf("Multiplication result:\n");
 *       saul_print_matrix(c);
 *       saul_free_matrix(c);
 *   }
 *   
 *   saul_free_matrix(a);
 *   saul_free_matrix(b);
 * 
 * Example - Using callbacks for custom operations:
 * 
 *   void print_element(Matrix *m, int i, int j) {
 *       float val = saul_get_value_by_index(m, i, j);
 *       printf("Element [%d][%d] = %.2f\n", i, j, val);
 *   }
 *   
 *   void scale_elements(Matrix *m1, Matrix *m2, int i, int j) {
 *       float val = saul_get_value_by_index(m1, i, j);
 *       saul_matrix_set_value(m1, i, j, val * 2.0f);  // Scale by 2
 *   }
 *   
 *   Matrix *m = saul_new_matrix(2, 2);
 *   // ... fill matrix ...
 *   
 *   // Print all elements
 *   saul_matrix_for_each(m, print_element);
 *   
 *   // Apply scaling operation
 *   saul_matrix_for_each_double(m, m, scale_elements);
 *   
 *   saul_free_matrix(m);
 * 
 * Example - Safe element access:
 * 
 *   Matrix *m = saul_new_matrix(3, 3);
 *   
 *   // Safe setting with boundary check
 *   if (saul_matrix_set_value(m, 2, 2, 5.0f) == 0) {
 *       printf("Value set successfully\n");
 *   }
 *   
 *   // Safe getting with boundary check
 *   if (saul_check_boundaries(m, 1, 1) > 0) {
 *       float val = saul_get_value_by_index(m, 1, 1);
 *       printf("Value at [1][1]: %.2f\n", val);
 *   }
 *   
 *   saul_free_matrix(m);
 * 
 * Note: Matrix addition and subtraction modify the first matrix in-place.
 *       Matrix multiplication returns a new matrix that must be freed.
 *       Always check return values for error conditions.
 * 
 * Return codes:
 *  0 or positive: Success
 * -1: Error (dimension mismatch, out of bounds, etc.)
 *  NULL: Memory allocation failure or invalid operation
 */

#ifndef INCLUDE_SAUL_H
#define INCLUDE_SAUL_H


#ifdef SAUL_IMPLEMENTATION
#include <malloc.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    int rows;
    int cols;
    float **items;
} Matrix;

typedef void (* saul_call_back)(Matrix *, int, int);
typedef void (* saul_call_back_double_matrix)(Matrix *, Matrix *, int, int);


// -- Setup/End
Matrix *saul_new_matrix(int rows, int cols);
void saul_free_matrix(Matrix *m);

// -- Utilities
int saul_matrix_set_value(Matrix *m, int i, int j, float value);
float saul_get_value_by_index(Matrix *m, int i, int j);
void saul_matrix_for_each(Matrix *m, saul_call_back cb);
int saul_matrix_for_each_double(Matrix *m1, Matrix *m2, saul_call_back_double_matrix cb);
int saul_check_boundaries(Matrix *m, int i, int j);

// -- Operations
int saul_matrix_add(Matrix *m1, Matrix *m2);
int saul_matrix_sub(Matrix *m1, Matrix *m2);
Matrix *saul_matrix_mul(Matrix *m1, Matrix *m2);

int saul_check_boundaries(Matrix *m, int i, int j) {
    if(m->cols <= j || i < 0 ) return -1;

    if(m->rows <= i || j < 0) return -1;

    return 1;
}

void saul_matrix_for_each(Matrix *m, saul_call_back cb) {
    for(int i = 0; i < m->rows; i++) {
        for(int j = 0; j < m->cols; j++) {
            cb(m, i, j);
        }
    }
}

int saul_matrix_for_each_double(Matrix *m1, Matrix *m2, saul_call_back_double_matrix cb) {
    if(m1->rows != m2->rows || m1->cols != m2->cols) {
        return -1;
    }

    for(int i = 0; i < m1->rows; i++) {
        for(int j = 0; j < m1->cols; j++) {
            cb(m1, m2, i, j);
        }
    }

    return 0;
}

Matrix *saul_new_matrix(int rows, int cols) {
    Matrix *m = (Matrix *)malloc(sizeof(Matrix)); 

    m->cols = cols;
    m->rows = rows;
    m->items = (float **)malloc(rows * sizeof(float *));

    for(int i = 0; i < rows; i++) {
        m->items[i] = (float *)malloc(cols * sizeof(float));
        for(int j = 0; j < cols; j++) {
            m->items[i][j] = 0;
        }
    }
    return m;
}

int saul_matrix_set_value(Matrix *m, int i, int j, float value) {

    if(saul_check_boundaries(m, i, j) < 0) {
        return -1;
    }

    m->items[i][j] = value;
    return 0;
}

float saul_get_value_by_index(Matrix *m, int i, int j) {
    if(saul_check_boundaries(m, i , j) < 0) {
        return -1;
    }

    return m->items[i][j];
}


// --------------------------------------- PRIVATE

static inline void saul_private_add_each(Matrix *m1, Matrix *m2, int i, int j) {
    int a = saul_get_value_by_index(m1, i, j);
    int b = saul_get_value_by_index(m2, i, j);
    saul_matrix_set_value(m1, i, j, a + b);
}

static inline void saul_private_sub_each(Matrix *m1, Matrix *m2, int i, int j) {
    int a = saul_get_value_by_index(m1, i, j);
    int b = saul_get_value_by_index(m2, i, j);
    saul_matrix_set_value(m1, i, j, a - b);
}
// ---------------------------------------


int saul_matrix_add(Matrix *m1, Matrix *m2) {
    if(m1->rows != m2->rows || m1->cols != m2->cols) {
        return -1;
    }
    saul_matrix_for_each_double(m1, m2, saul_private_add_each);
    return 0;
}

int saul_matrix_sub(Matrix *m1, Matrix *m2) {
    if(m1->rows != m2->rows || m1->cols != m2->cols) {
        return -1;
    }
    saul_matrix_for_each_double(m1, m2, saul_private_sub_each);
    return 0;
}

Matrix *saul_matrix_mul(Matrix *m1, Matrix *m2) {

    if(m1->cols != m2->rows) {
        return NULL;
    }

    Matrix *m3 = saul_new_matrix(m1->rows, m2->cols);

    for(int i = 0; i < m1->rows; i++) {
        for(int j = 0; j < m2->cols; j++) {

            for(int k = 0; k < m1->cols; k++) {
                float a = saul_get_value_by_index(m1, i, k);
                float b = saul_get_value_by_index(m2, k, j);
                float current = saul_get_value_by_index(m3, i, j);

                float value =  current + a * b;
                saul_matrix_set_value(m3, i, j, value);
            }

        }
    }
    return m3;
}

void saul_print_matrix(Matrix *m) {
    int col_width[m->cols];

    for(int j = 0; j < m->cols; j++) {
        int max = 0;
        for(int i = 0; i < m->rows; i++) {
            char buf[64];
            snprintf(buf, sizeof(buf), "%.2f", m->items[i][j]);
            int len = strlen(buf);
            if(len > max) max = len;
        }
        col_width[j] = max;
    }

    for(int i = 0; i < m->rows; i++) {
        printf("[");
        for(int j = 0; j < m->cols; j++) {
            printf(" %*.*f ", col_width[j], 2, m->items[i][j]);
        }
        printf("]\n");
    }
}



void saul_free_matrix(Matrix *m) {
    free(m);
}


#endif // SAUL_IMPLEMENTATION
#endif // INCLUDE_SAUL_H