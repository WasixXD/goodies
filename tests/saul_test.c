#define PICKY_IMPLEMENTATION
#define SAUL_IMPLEMENTATION
#include <saul.h>
#include <picky.h>



void matrix_setup_test(T *t) {
    picky_test(t, "saul_new_matrix() don't return null");

    Matrix *m = saul_new_matrix(3, 4);
    picky_assertNotNull(t, m);


    picky_test(t, "saul_new_matrix() returns correct cols");
    picky_int_toBe(t, 3, m->rows);

    picky_test(t, "saul_new_matrix() returns correct rows");
    picky_int_toBe(t, 4, m->cols);


    saul_free_matrix(m);
}

void each(Matrix *m, int i, int j) {
    m->items[i][j] = 1;
}

void matrix_utilities_test(T *t) {

    Matrix *m = saul_new_matrix(3, 4);

    int i = 1;
    int j = 1;

    picky_test(t, "saul_matrix_set_value()");
    saul_matrix_set_value(m, i, j, 4.5f);
    picky_float_toBe(t, 4.5f, m->items[i][j]);


    picky_test(t, "saul_get_value_by_index()");
    float actual = saul_get_value_by_index(m, 0, 0);
    picky_float_toBe(t, 0, actual);

    picky_test(t, "saul_matrix_for_each() set every value to 1.0");
    saul_matrix_for_each(m, each);
    picky_float_toBe(t, 1.0f, m->items[0][0]);

    picky_test(t, "saul_check_boundaries()");
    picky_assert(t, saul_check_boundaries(m, 6, 6) < 0);

    picky_test(t, "saul_is_upper_triangular() should fail");
    picky_assert(t, saul_is_upper_triangular(m) < 0);


    picky_test(t, "saul_is_upper_triangular() should pass");
    saul_matrix_set_value(m, 1, 0, 0);
    saul_matrix_set_value(m, 2, 0, 0);
    saul_matrix_set_value(m, 2, 1, 0);
    picky_assert(t, saul_is_upper_triangular(m) >= 0);


    saul_free_matrix(m);
}

void fill_with_two(Matrix *m, int i, int j) {
    saul_matrix_set_value(m, i, j, 2);
}

void matrix_operations_test(T *t) {
    Matrix *m1 = saul_new_matrix(3, 3);
    Matrix *m2 = saul_new_matrix(3, 3);

    saul_matrix_for_each(m1, fill_with_two);
    saul_matrix_for_each(m2, fill_with_two);

    picky_test(t, "saul_matrix_add()");
    saul_matrix_add(m1, m2);
    picky_float_toBe(t, saul_get_value_by_index(m1, 0, 0), 4);


    picky_test(t, "saul_matrix_sub()");
    saul_matrix_sub(m1, m2);
    picky_float_toBe(t, saul_get_value_by_index(m1, 0, 0), 2);


    picky_test(t, "saul_matrix_mul()");
    Matrix *m3 = saul_matrix_mul(m1, m2);
    picky_float_toBe(t, saul_get_value_by_index(m3, 0, 0), 12);

    picky_test(t, "saul_matrix_mul() returned a new matrix");
    picky_assert(t, m3->cols == m1->cols && m3->rows == m2->rows);

    picky_test(t, "saul_matrix_transpose()");
    Matrix *m4 = saul_new_matrix(3, 2);
    saul_matrix_transpose(&m4);
    picky_assert(t, m4->rows == 2 && m4->cols == 3);

    saul_matrix_set_value(m4, 0, 0, 2);
    saul_matrix_set_value(m4, 0, 1, -3);
    saul_matrix_set_value(m4, 0, 2, 10);

    saul_matrix_set_value(m4, 1, 0, 7);
    saul_matrix_set_value(m4, 1, 1, 10);
    saul_matrix_set_value(m4, 1, 2, 4);
    picky_test(t, "saul_gauss_reduction()");
    picky_assert(t, saul_is_upper_triangular(m4));

    saul_free_matrix(m1);
    saul_free_matrix(m2);
    saul_free_matrix(m3);
    saul_free_matrix(m4);
}

int main(int argc, char **argv) {
    picky_describe("Matrix Setup Testing", matrix_setup_test);
    picky_describe("Matrix Utilities Testing", matrix_utilities_test);
    picky_describe("Matrix Operations Testing", matrix_operations_test);
}