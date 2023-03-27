#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include "shuffle.h" //implements
#include "exchange.h"



/**
 ** @tie: tie breaker for when a part is not cleanly divisible by 2.
 ** @a, @b: Sizes of the segments A and B. Availables to the entire
 ** recursion stack.
 **/
static unsigned int tie;
static size_t a, b;

struct Shf{
    double *arr; //array to shuffle
    size_t spp;  // segments per part
    size_t thr;  // threads to use
};



/**
 ** Single-threaded Recursive shuffler (pre-order tree traversal)
 **/
static void shuffle_recursive_st(double *arr, size_t spp){
    if(spp < 2) // Basecase. See /docs/shuffle/shuffle_ge.pdf pp4
        return;
    size_t ch_spp = spp>>1;
    double *exch_arr, *lch_arr = arr, *rch_arr;
    size_t exch_arr_l, exch_x_l, lch_spp, rch_spp;
    
    if(spp & 1ULL){ // spp cannot be exacly divided into two childs
        exch_arr = arr + (ch_spp + tie) * a;
        exch_x_l = (ch_spp + 1 - tie) * a;
        exch_arr_l = exch_x_l + (ch_spp + tie) * b;
        rch_arr = arr + (ch_spp+tie) * (a+b);
        lch_spp = ch_spp + tie;
        rch_spp = ch_spp + (tie^1U);
    }else{ // if spp can be exactly divided in 2
        exch_arr = arr + ch_spp * a;
        exch_x_l = ch_spp * a;
        exch_arr_l = ch_spp * (a + b);
        rch_arr = arr + ch_spp * (a+b);
        lch_spp = rch_spp = ch_spp;
    }
    exchange(exch_arr, exch_arr_l, exch_x_l, 1);
    shuffle_recursive_st(lch_arr, lch_spp);
    shuffle_recursive_st(rch_arr, rch_spp);
    return;
}


/**
 ** Single-threaded Recursive unshuffler (post-order tree traversal)
 **/
static void unshuffle_recursive_st(double *arr, size_t spp){
    if(spp < 2) // Base-case
        return;

    size_t ch_spp = spp>>1;
    double *exch_arr, *lch_arr = arr, *rch_arr;
    size_t exch_arr_l, exch_x_l, lch_spp, rch_spp;
    if(spp & 1ULL){
        exch_arr = arr + (ch_spp + tie) * a;
        exch_x_l = (ch_spp + tie) * b;
        exch_arr_l = exch_x_l + (ch_spp + (tie^1U)) * a;
        rch_arr = arr + (ch_spp+tie) * (a+b);
        lch_spp = ch_spp + tie;
        rch_spp = ch_spp + (tie^1U);
    }else{
        exch_arr = arr + ch_spp * a;
        exch_x_l = ch_spp * b;
        exch_arr_l = ch_spp * (a + b);
        rch_arr = arr + ch_spp * (a+b);
        lch_spp = rch_spp = ch_spp;
    }
    unshuffle_recursive_st(lch_arr, lch_spp);
    unshuffle_recursive_st(rch_arr, rch_spp);
    exchange(exch_arr, exch_arr_l, exch_x_l, 1);
    return;
}

/**
 ** Multi-threaded implementation helpers
 **/
static void shuffle_recursive(double *arr, size_t spp, size_t thr);
static void unshuffle_recursive(double *arr, size_t spp, size_t thr);
static void *thread_shuffle(void *arg) {
    struct Shf *s = (struct Shf *)arg;
    shuffle_recursive(s->arr, s->spp, s->thr);
    pthread_exit(NULL);
}
static void *thread_unshuffle(void *arg) {
    struct Shf *s = (struct Shf *)arg;
    unshuffle_recursive(s->arr, s->spp, s->thr);
    pthread_exit(NULL);
}


/**
 ** Recursive Shuffler:
 ** This Function Does Two Things: 
 **  - exchange the central portions of the incomming array @arr.
 **  - call itself (shuffle) on each "half" of the array @arr, so the 
 **    shuffle gets recursively performed.
 ** This produces a pre-order traversal of a binary tree of exchange operations.
 **
 ** The complexity of the code pretty much resides in:
 **  - distributing the available threads @thr.
 **  - dealing with cases where you cannot split the @arr evenly.
 **  - calling a single-threaded version of the routine when
 **    there is no more threads to exploit.
 ** 
 ** @arr: array with the data.
 ** @spp: segments per part (h in the paper).
 ** @thr: threads available to the routine.
 **/
static void shuffle_recursive(double *arr, size_t spp, size_t thr){
    if(spp < 2) // Base-case. See pp4 of /docs/shuffle/shuffle_ge.pdf
        return;
    
    // CHildrens' shuffle Segments Per Part
    size_t ch_spp;
    ch_spp = spp>>1; // floor(spp/2)
    
    // Parameters for Exchange:
    //         ┎─────── exch_arr: where to start the exchange
    //         ┃     ┌─ exch_arr_l: length of exch_arr
    //         ┠─────┴───┐
    // [ AAAAA AAAA BBBBB BBBB ]
    //         └─┬─┘ 
    //           └───── exch_x_l: length of the first part of exch_arr
    double *exch_arr;
    size_t exch_arr_l, exch_x_l;

    // Parameters for Left and Right child shuffles:
    //   ┎──────────────────── arr: beginning of the L array
    //   ┃       ┌──────────── lch_spp: number of segments per part
    //   ┃     ┌┬┼┬┐
    // [ AAAAA BBBBB AAAA BBBB ]
    //               ┃    └┼┴┘ 
    //               ┃     └── rch_spp: number of segments per part
    //               ┖──────── rch_arr: beginning of the R array
    double *lch_arr, *rch_arr;
    size_t lch_spp, rch_spp;
    lch_arr = arr;

    // setup parameters
    if(spp & 1ULL){ // spp cannot be evenly distributed among the two childs
        exch_arr = arr + (ch_spp + tie) * a;
        exch_x_l = (ch_spp + 1 - tie) * a;
        exch_arr_l = exch_x_l + (ch_spp + tie) * b;
        rch_arr = arr + (ch_spp+tie) * (a+b);
        lch_spp = ch_spp + tie;
        rch_spp = ch_spp + (tie^1U);
    }
    else{ // if spp can be exactly divided in 2
        exch_arr = arr + ch_spp * a;
        exch_x_l = ch_spp * a;
        exch_arr_l = ch_spp * (a + b);
        rch_arr = arr + ch_spp * (a+b);
        lch_spp = rch_spp = ch_spp;
    }
    
    // perform exchange, then call left and right shuffle
    exchange(exch_arr, exch_arr_l, exch_x_l, thr);
    if(thr < 2){
        shuffle_recursive_st(lch_arr, lch_spp);
        shuffle_recursive_st(rch_arr, rch_spp);
        return;
    }else{ // consider thr = 2t+1.
        // odd(thr) && A>B ? t+1 : t;
        size_t lthr = (thr>>1) + (thr & 1ULL & tie);
        // odd(thr) && A<B ? t+1 : t;
        size_t rthr = (thr>>1) + (thr & 1ULL & (tie^1U));
        struct Shf lch={.arr=lch_arr, .spp=lch_spp, .thr=lthr};
        pthread_t left;
        pthread_create(&left, NULL, &thread_shuffle, (void*)&lch);
        shuffle_recursive(rch_arr, rch_spp, rthr);
        pthread_join(left, NULL);
        return;
    }
}

static void unshuffle_recursive(double *arr, size_t spp, size_t thr){
    if(spp < 2) // Base-case
        return;

    size_t ch_spp;
    double *exch_arr;
    size_t exch_arr_l, exch_x_l;
    double *lch_arr, *rch_arr;
    size_t lch_spp, rch_spp;
    ch_spp = spp>>1;
    lch_arr = arr;
    if(spp & 1ULL){
        exch_arr = arr + (ch_spp + tie) * a;
        exch_x_l = (ch_spp + tie) * b;
        exch_arr_l = exch_x_l + (ch_spp + (tie^1U)) * a;
        rch_arr = arr + (ch_spp+tie) * (a+b);
        lch_spp = ch_spp + tie;
        rch_spp = ch_spp + (tie^1U);
    }else{
        exch_arr = arr + ch_spp * a;
        exch_x_l = ch_spp * b;
        exch_arr_l = ch_spp * (a + b);
        rch_arr = arr + ch_spp * (a+b);
        lch_spp = rch_spp = ch_spp;
    }

    // call left and right unshuffle, then exchange
    if(thr < 2){
        unshuffle_recursive_st(lch_arr, lch_spp);
        unshuffle_recursive_st(rch_arr, rch_spp);
    }else{ // consider thr = 2t+1.
        size_t lthr = (thr>>1) + (thr & 1ULL & tie);
        size_t rthr = (thr>>1) + (thr & 1ULL & (tie^1U));
        struct Shf lch={.arr=lch_arr, .spp=lch_spp, .thr=lthr};
        pthread_t left;
        pthread_create(&left, NULL, &thread_unshuffle, (void*)&lch);
        unshuffle_recursive(rch_arr, rch_spp, rthr);
        pthread_join(left, NULL);
    }
    exchange(exch_arr, exch_arr_l, exch_x_l, thr);
    return;
}


/**
 ** A and B are segments of normally different length.
 ** AAAAAAAA and BBBBBBBB are the two parts the array is divided into. 
 ** Cuts each part in two, and exchange the middle portions, so each 
 ** new part of the array now has everything it needs to keep shuffling:
 ** 
 ** [ AAAAAAAA BBBBBBBB ] -> [ AAAABBBB AAAABBBB ]
 ** 
 ** then it applies recursion to shuffle each new part:
 **
 ** [ AAAABBBB AAAABBBB ] -> [ AABB AABB   AABB AABB]
 **
 ** If parts are not divisible by 2 (h % 2 = 1), the tie is broken in
 ** favor of moving one more segment from the smallest A or B.
 **/
inline void shuffle(double *arr, size_t h, size_t a_sz, size_t b_sz,
                    size_t thr){
    if(h < 2 || a_sz == 0 || b_sz == 0)
        return;
    a = a_sz;
    b = b_sz;
    tie = a > b ? 1 : 0;    
    shuffle_recursive(arr, h, thr);
}

/**
 ** Inverse of the shuffle operation.
 **/
inline void unshuffle(double *arr, size_t h, size_t a_sz, size_t b_sz,
                      size_t thr){
    if(h < 2 || a_sz == 0 || b_sz == 0)
        return;
    a = a_sz;
    b = b_sz;
    tie = a > b ? 1 : 0;
    unshuffle_recursive(arr, h, thr);
}

/* end exch-geom.c */
