#include <pthread.h>
#include "exchange.h" //implements

size_t EXCH_MIN_GRAIN; // unused here

struct td{
    double *arr;
    double *brr;
    size_t len;
};

/**
 ** Mirrors an array.
 **/
inline static void mirror_st(double *arr, size_t arr_len){
    register double tmp;
    size_t left = 0, right = arr_len - 1;
    while(right > left){
        tmp = arr[left];
        arr[left] = arr[right];
        left++;
        arr[right] = tmp;
        right--;
    }
}

/**
 ** Cyclically shifts @arr upwards (or to the left, if you want to see
 ** it that way)
 ** Example:
 ** O X X X X -> X X X X O
 **/
inline static void shift_up(double *arr, size_t arr_len){
    size_t i;
    register double tmp;
    tmp=arr[0];
    for(i=1;i<arr_len;++i){
        arr[i-1] = arr[i];
    }
    arr[arr_len-1] = tmp;
}

/**
 ** Cyclically shifts @arr downwards (or to the right, if you want to
 ** see it that way)
 ** Example:
 ** X X X X O -> O X X X X
 **/
inline static void shift_down(double *arr, size_t arr_len){
    size_t i;
    register double tmp;
    tmp=arr[arr_len-1];
    for(i=arr_len-1;i>0;--i)
        arr[i] = arr[i-1];
    arr[0] = tmp;
}

/**
 ** Exchange the two parts X and Y of a vector by mirroring X,
 ** then mirroring Y, and finally mirroring the full vector.
 **/
inline void exchange(double *arr, size_t arr_len, size_t xsize, size_t thr){
    size_t ysize = arr_len - xsize;
    
    if(xsize == 0 || ysize == 0)
        return;
    if(xsize == 1){
        shift_up(arr, arr_len);
    }else if(ysize == 1){
        shift_down(arr, arr_len);
    }else{
        mirror_st(arr, xsize); // mirror X part
        mirror_st(arr + xsize, ysize); // mirror Y part
        mirror_st(arr, arr_len); // mirror all.
    }
}
