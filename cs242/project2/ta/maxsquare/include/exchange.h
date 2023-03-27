#ifndef EXCHANGE_H_
#define EXCHANGE_H_

#include <stddef.h>
#include "vector.h"

extern size_t EXCH_MIN_GRAIN;

/**
 ** Exchange two contiguous parts of a vector.
 ** @arr    : array to exchange.
 ** @arr_len: length of the array.
 ** @xsize  : length of the first part.
 ** @threads: threads to use in the exchange.
 ** Example:
 ** XXYYYY -> YYYYXX
 **/
extern void exchange(double *arr, size_t arr_len, size_t xsize, size_t threads);

#endif
