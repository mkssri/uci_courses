#ifndef TRANSPOSE_H_
#define TRANSPOSE_H_

#include <stddef.h>

extern void transpose(double **mat, size_t mat_rows, size_t mat_cols,
                      size_t thr);

#endif
