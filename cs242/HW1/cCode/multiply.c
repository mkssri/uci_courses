
#include <stdio.h>
#include <stdlib.h>
#include "multiply.h"

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
    //assume it is 64 bytes a line, where a int is 4 bytes
    //My machine
    /*  way of associativity = 12
        size = 48k
        line size 64
        get to it by /sys/devices/system/cpu/
        //one page is 4000, about 62 lines it total
    */
    int n = A->n;
    for(int i = 0; i < n;){
        for(int j = 0; j < n;){
            for(int irow = i; (irow < i+16 && irow < n); irow++){
                for(int jcolumn = j; (jcolumn < i+16 && jcolumn < n); jcolumn++){
                    for(int k = 0; k< n; k++){
                        C->ptr[irow * n + jcolumn] += A->ptr[irow * n + k] * B->ptr[k * n + jcolumn];
                    }
                }
            }
            i += 16;
            j += 16;
        }
    }
    return;
    //TODO write a cache one
}

void multithreadedMulti(Mat* A, Mat* B, Mat* C){
    //TODO write the multithreaded one
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