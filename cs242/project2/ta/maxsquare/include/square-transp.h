#ifndef TRANSPOSE_SQUARE_H_
#define TRANSPOSE_SQUARE_H_

#include <stddef.h>

/**
 ** Minimum per-thread workload (memory elements) to allow multi threaded
 ** square matrix transposition.
 **/
extern size_t SQTR_MIN_GRAIN;

/**
 ** Transpose a square matrix
 **/
extern void *transpose_square(double *mat, size_t mat_rows, size_t thr);

#endif
