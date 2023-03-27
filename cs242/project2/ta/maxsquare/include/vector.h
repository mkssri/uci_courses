/**
 ** Vector library:
 ** Contains functions to: create, fill, copy, compare, print, and
 ** destroy Vectors and Matrices.
 ** Everything is pretty much what you would expect from the name of the
 ** function, except the print function, when the vector/matrix is too
 ** big, it just prints the tips/corners of the vector/matrix, so you
 ** can get an idea of what is in the data structure.
 **/

#ifndef VECTOR_H_
#define VECTOR_H_

#include<stddef.h>

typedef struct{
    double *ptr;
    size_t n;
} Vec;

typedef struct{
    double *ptr;
    size_t m;
    size_t n;
} Mat;

/**
 ** Allocates memory for a vector of size @n
 **/
void vec_create(Vec *vec, size_t n);
void mat_create(Mat *mat, size_t m, size_t n);

/**
 ** Deallocates memory for @vec sets vec->ptr to NULL,
 ** and sets its dimension vec->n to 0
 **/
void vec_destroy(Vec *vec);
void mat_destroy(Mat *mat);

/**
 ** Fills @vec with consecutive positive numbers, or:
 ** _val  : With a single particular value @val.
 ** _parts: The first @first elements are filled with consecutive non negative
 **         integers (starting with 0), the remaining with consecutive negative
 **         integers.
 ** _rand : With random non negative integers.
 **/
void vec_fill(Vec *vec);
void vec_fill_val(Vec *vec, double val);
void vec_fill_parts(Vec *vec, size_t first);
void vec_fill_rand(Vec *vec);

/**
 ** Fills @mat with consecutive positive numbers, or:
 ** _val  : With a single particular value @val.
 ** _diag : Fills diagonal with @val.
 ** _top  : Fills the first @rows columns with consecutive non negative
 **         integers (starting with 0), the remaining with consecutive negative
 **         integers.
 ** _left : Fills the first @cols columns with consecutive non negative
 **         integers (starting with 0), the remaining with consecutive negative
 **         integers.
 ** _coor : Fills each cell with its coordinates.
 ** _rand : With random non negative integers.
 **/
void mat_fill(Mat *mat);
void mat_fill_val(Mat *mat, double val);
void mat_fill_diag(Mat *mat, double val);
void mat_fill_top(Mat *mat, size_t rows);
void mat_fill_left(Mat *mat, size_t cols);
void mat_fill_coor(Mat *mat);
void mat_fill_rand(Mat *mat);

/**
 ** Copy vector @from to vector @to
 **/
void vec_copy(Vec *to, Vec *from);
void mat_copy(Mat *to, Mat *from);

/**
 ** Scalar, Vector and Matrix multiplications.
 ** _rmult is a row-wise multiplication, So A->n matches BT->n
 **/
void sv_mult(double c, Vec *A, int thr);
void sm_mult(double c, Mat *A, int thr);

void vv_dot(double *c, Vec *A, Vec *B);
void vv_mult(Vec *C, Vec *A, Vec *B);
void vm_mult(Vec *C, Vec *A, Mat *B);
void mv_mult(Vec *C, Mat *A, Vec *B);

void mm_mult(Mat *C, Mat *A, Mat *B);
void mm_rmult(Mat *C, Mat *A, Mat *BT);


/**
 ** Returns 0 if @v1 and @v2 have the same size and contain the same
 ** values.
 ** Returns 1 if @v1 and @v2 have the same size but different values.
 ** In this case, @diff gets the index of the first differing value.
 ** Returns 2 if @v1 and @v2 have different sizes.
 ** For 0 and 2 return, the value of @diff is undetermined.
 **/
int vec_diff(Vec *v1, Vec *v2, size_t *diff);
int mat_diff(Mat *m1, Mat *m2, size_t *diff);

/**
 ** Print the whole vector in screen. If the vector is too
 ** large, it prints a representative sample of it
 **/
void vec_print(Vec *vec);
void mat_print(Mat *mat);

#endif
