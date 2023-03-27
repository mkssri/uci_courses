#ifndef SHUFFLE_H_
#define SHUFFLE_H_

#include <stddef.h>

/**
 ** In-Place Shuffle or unshuffle an array @arr composed of 2*@h
 ** segments. @h segments of size @a, and @h segments of size @b.
 ** If @thr > 1, a multi-threaded implementation is used.
 ** 
 ** shuffle example input:
 ** arr = [ aaa aaa aaa aaa aaa bbbb bbbb bbbb bbbb bbbb ], h=5, a=3, b=4
 ** output:
 ** arr = [ aaa bbbb aaa bbbb aaa bbbb aaa bbbb aaa bbbb ]
 **/
extern void shuffle(double *arr, size_t h, size_t a, size_t b, size_t thr);

/**
 ** unshuffle example input:
 ** arr = [ aa b aa b aa b aa b ], h=4, a=2, b=1
 ** output:
 ** arr = [ aa aa aa aa b b b b ]
 **/
extern void unshuffle(double *arr, size_t h, size_t a, size_t b, size_t thr);

#endif
