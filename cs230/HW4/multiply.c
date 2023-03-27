#include <stdio.h> //remove if not using.
#include <stdlib.h>
#include <pthread.h>

#include "util.h"//implementing

// typedef struct { //example structure
//     int example;
//     int e_g;
// } Example_Structure;

// static void example_helper_function(int n){
//     // Functions defined with the modifier 'static' are only visible
//     // to other functions in this file. They cannot be called from
//     // outside (for example, from main.c). Use them to organize your
//     // code. Remember that in C, you cannot use a function until you
//     // declare it, so declare all your utility functions above the
//     // point where you use them.
//     //
//     // Maintain the mat_sq_trans_xt functions as lean as possible
//     // because we are measuring their speed. Unless you are debugging,
//     // do not print anything on them, that consumes precious time.
//     //
//     // You may delete this example helper function and structure, and
//     // write your own useful ones.

//     Example_Structure es1;
//     es1.example = 13;
//     es1.e_g = 7;
//     printf("n = %d\n", es1.example + es1.e_g + n);
//     return;
// }


/*
Multi Threaded Part
*/


//Structure for Data in thread
typedef struct {
    Mat *A;
    Mat *B;
    Mat *C;
    int r;
    pthread_mutex_t *mutexData;
}DataInThread_th;

/*
This is the function that assigns rows to each thread for performing
matrix multiplication.
*/
void* execute_in_th_func(void *args) {

    DataInThread_th *thData = (DataInThread_th *)args;
    
    int r  = 0;
    int n = thData->A->n;
    int m = thData->A->m;
    Mat *C = thData->C;
    Mat *A = thData->A;
    Mat *B = thData->B;


    while(r<m){
        pthread_mutex_lock(thData->mutexData); //mutex data lock (other threads cant access the data)
        r = (thData->r)++;
        pthread_mutex_unlock(thData->mutexData); //mutex data unlock (other threads can access the data)

        if(r < m) {
            int j=0, k=0;
            for(j = 0; j < n; j++) {
                for(k = 0; k < n; k++) {
                    int a,b,c;
                    a = r*m + j;
                    b = j + k*m;
                    c = k + r*m;
                    C->ptr[a] += B->ptr[b] * A->ptr[c];
                }
            }
        }
    }

    pthread_exit(NULL);
}


/*
Below function is multi threaded implementation for matrix multiplication
*/
void mat_multiply(Mat *A, Mat *B, Mat *C, unsigned int threads){
    //Put your code here.
    //Remember to set the correct values for C->m, and C->n
    //after doing the multiplication.
    //example_helper_function(1000);
    pthread_t *arrOfThreads;
    arrOfThreads = (pthread_t *)malloc(sizeof(pthread_t) * threads);
    pthread_mutex_t mutexData;
    pthread_mutex_init(&mutexData, NULL);

    DataInThread_th thData;

    thData.mutexData = &mutexData;
    thData.r = 0;
    thData.C = C;
    thData.A = A;
    thData.B = B;

    int x=0;
    for(x = 0; x < threads; x++) {
        pthread_create(arrOfThreads + x, NULL, execute_in_th_func, (void *)&thData);
    }
    for(x = 0; x < threads; x++) {
        pthread_join(arrOfThreads[x], NULL);
    }

    return;
}

/*
Sequential Part
*/
// double get_ele_frm_matrix(Mat *matrix, int i, int j){
//     return matrix->ptr[matrix->m * i + j];
// }

// void add_val_to_matrix(Mat *matrix, int i, int j, double val) {
//     matrix->ptr[matrix->m * i + j] += val;
// }

// void mat_multiply_sequential(Mat *A, Mat *B, Mat *C) {
//     int i=0,j=0,k=0;
//     for(i = 0; i < A->m; i++){
//         for(j = 0; j < A->n; j++){
//             for(k = 0; k < A->n; k++){
//                 double val = get_ele_frm_matrix(B, k, j) * get_ele_frm_matrix(A, i, k);
//                 add_val_to_matrix(C, i, j, val);
//             }
//         }
//     }
// }
