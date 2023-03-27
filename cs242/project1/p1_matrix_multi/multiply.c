
#include <stdio.h>
#include <stdlib.h>
#include "multiply.h"
#include<pthread.h>

void mat_init(Mat *mat, unsigned int m, unsigned int n){
    if(m == 0 || n == 0){
	printf("mat_init: Matrix size invalid.\n");
	exit(1);
    }
    mat->ptr = (int*) calloc(m*n, sizeof(int));
    if(mat->ptr == NULL){
	printf("mat_init: Couldn't allocate memory for matrix.\n");
	exit(1);
    }
    mat->m = m;
    mat->n = n;
    return;
}

void mat_fill(Mat *mat){
    for(int i=0; i < mat->m*mat->n; i++){
	mat->ptr[i] = (double)i;
    }
    return;
}

void mat_diag(Mat *mat, double diag){
    if(mat == NULL){
	printf("mat_diag: The matrix is NULL.\n");
	exit(1);
    }
    if(mat->ptr == NULL && mat->m > 0 && mat->n > 0){
	printf("mat_diag: The matrix is not initialized.\n");
	exit(1);
    }
    unsigned int off = 0, max_off= mat->m * mat->n;
    while(off < max_off){
	mat->ptr[off] = diag;
	off += mat->n + 1;
    }
    return;
}

void mat_copy(Mat *to, Mat *from){
    if(from == NULL || to == NULL){
	printf("mat_copy: one of the matrices is NULL.\n");
	exit(1);
    }
    if(from->ptr == NULL || to->ptr == NULL){
	printf("mat_copy: one of the matrices is not initialized.\n");
	exit(1);
    }
    
    if(from->m != to->m || from->n != to->n){
	printf("mat_copy: matrices size does not match.\n");
	exit(1);
    }
    
    for(int i=0; i < from->m*from->n; i++){
	to->ptr[i] = from->ptr[i];
    }
    return;
}

void mat_print(Mat *mat){
    unsigned int i, j, m = mat->m, n = mat->n;
    for(i=0; i < m; i++){
	for(j=0; j < n; j++){
	    printf("  %5.1d", mat->ptr[i*n + j]);
	}
	printf("\n");
    }
    return;
}

void randomize(Mat *mat)
{
    for(int i=0; i < mat->m*mat->n; i++){
	    mat->ptr[i] = rand()%10;
    }
    return;
}


void fillZeros(int n, int (*a)[n])
{
    for(int i=0; i<n; i++) {
        for(int j=0;j<n;j++) {
            a[i][j] = 0;
        }
    }
}

void printM(int n, int (*a)[n])
{
    for(int i=0; i<n; i++) {
        for(int j=0;j<n;j++) {
            printf("%d ", a[i][j]);
            if(j==n-1){
                printf("\n");
            }
        }
    }
}


void singleThreadedMuliCache(Mat* A, Mat* B, Mat* C){
    int n = A->n;
    int Bcolj[n];
    for (int j = 0; j < n; j++)
    {
        for (int k = 0; k < n; k++){
            Bcolj[k] = B->ptr[k * n + j];
        }
        for (int i = 0; i < n; i++){
            int sum = 0;
            for (int k = 0; k < n; k++)
                sum +=  A->ptr[i * n + k] * Bcolj[k];
            C->ptr[i * n + j] = sum;
        }
   }
    return;
}

struct argument {
    Mat* A;
    Mat* B;
    Mat* C;
    unsigned int n;
    unsigned int threads;
} argument;

int global_row;

void *multiply_not_friendly (void* ptr) {
    struct argument* arg = (struct argument*) ptr;
    unsigned int n = arg->n;
    Mat* A = arg->A;
    Mat* B = arg->B;
    Mat* C = arg->C;

    int row = __atomic_fetch_add(&global_row, 1, __ATOMIC_SEQ_CST);
    while (row < n) {
        for (int col = 0; col < n; col++) {
            for (int k = 0; k < n; k++) {
                C->ptr[row * n + col] += A->ptr[row * n + k] * B->ptr[k * n + col];
            }
        }
        row = __atomic_fetch_add(&global_row, 1, __ATOMIC_SEQ_CST);
    }

    pthread_exit(NULL);
}


void *multiply_friendly (void* ptr) {
    struct argument* arg = (struct argument*) ptr;
    unsigned int n = arg->n;
    Mat* A = arg->A;
    Mat* B = arg->B;
    Mat* C = arg->C;
    unsigned int thread = arg->threads;
    int col = __atomic_fetch_add(&global_row, 1, __ATOMIC_SEQ_CST);
    int start = col * n/thread;
    int end = (col+1) * n/thread;
    if(col == thread-1) end = n;

    int Bcolj[n];
    int k, i;
    for (col = start; col < end; col++)
    {
        for (k = 0; k < n; k++){
            Bcolj[k] = B->ptr[k * n + col];
        }
        for (i = 0; i < n; i++){
            int sum = 0;
            for (k = 0; k < n; k++)
                sum +=  A->ptr[i * n + k] * Bcolj[k];
            C->ptr[i * n + col] = sum;
        }
   }
    
    pthread_exit(NULL);
}

void multithreadedMulti(Mat* A, Mat* B, Mat* C, unsigned int thread, unsigned int flag){
    global_row = 0;
    unsigned int n = A->m;
    pthread_t threads[thread];
    struct argument arg;
    arg.A = A;
    arg.B = B;
    arg.C = C;
    arg.n = n;
    arg.threads = thread;
    
    for (int i = 0; i < thread; i++) {
        if(flag == 0){
            pthread_create(&threads[i], NULL, multiply_not_friendly, (void*) &arg);
        }else{
            pthread_create(&threads[i], NULL, multiply_friendly, (void*) &arg);
        }
       
    }
    for (int i = 0; i < thread; i++)
        pthread_join(threads[i], NULL);
}


void singleThreadedMulti(Mat* A, Mat* B, Mat* C){
    int n = A->n;
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            for(int k = 0; k < n; k++){
                C->ptr[i * n + j] += A->ptr[i * n + k] * B->ptr[k * n + j];
            }
        }
    }
    return;
}
