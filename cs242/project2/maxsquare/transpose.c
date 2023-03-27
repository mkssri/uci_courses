#include <stdio.h>
#include <stdlib.h>
#include "transpose.h"
#include<pthread.h>
#include <time.h>
#include <omp.h>
#define omp_thread_count 4

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

void mat_equal(Mat *mat1, Mat *mat2) {

    unsigned int i, j;
    unsigned int m1 = mat1->m, n1 = mat1->n;
    unsigned int m2 = mat2->m, n2 = mat2->n;
    if (m1 != m2 || n1 != n2) {
        printf("not equal \n");
    }

    for (int i = 0; i < m1 * n2; i++) {
        if (mat1->ptr[i] != mat2->ptr[i]) {
            printf("not equal \n");
            return;
        }
    }
    printf("equal \n");
    return;
}

/*
    ========================================
    N x N Matrix Transposition
*/

/**
 * Naive Matrix Transpose
 * @brief Naive matrix transposition. Not in place and for square matrixs 
 */
void naive_mat_transpose(Mat *mat){
    int n = mat->n;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            int temp = mat->ptr[i * n + j];
            mat->ptr[i * n + j] = mat->ptr[j * n + i];
            mat->ptr[j * n + i] = temp;
            
        }
    }
    return;
}

/**
 * Matrix Transposition
 * @brief Inplace matrix transposition without multiplication or division
 */
void mat_transpose(Mat *mat){
    int upperHelper = 1;
    int upperIndex = 1;
    int lowerIndex;
    int n = mat->n;
    int i = n-1;
    while(i >= 1){
        //Sets indexs for the start of each column and row
        upperIndex = upperHelper;
        lowerIndex = upperIndex + n -1;

        //traverses each row or column
        while(upperIndex < upperHelper+i){
            int temp =  mat->ptr[upperIndex];
            mat->ptr[upperIndex] = mat->ptr[lowerIndex];
            mat->ptr[lowerIndex] = temp;
            upperIndex++;
            lowerIndex += n;
        }
        //increments to the next row or column
        upperHelper += n+1;
        i--;
    }
    return;
}
int counter;

/**
 * Square Matrix Cycles Helper
 * @brief TODO Add comments and explanation
 * 
 */
void SquareMTCyclesHelper(const unsigned rows, const unsigned columns, Mat *mat)
{
    
	int holder = __atomic_fetch_add(&counter, 1, __ATOMIC_SEQ_CST);
    unsigned start_num = holder * ((rows * columns )/omp_thread_count);
	unsigned array_size = start_num + (rows * columns)/omp_thread_count;
	for(int start = start_num; start < array_size; start++)
	{
		unsigned i = 1;
		unsigned next = (start % rows) * columns + start / rows;
		if (next > start )
		{
			const double tmp = mat->ptr[start];
			next = start;
			i = (next % rows) * columns + next / rows;
			while (i > start)
      			{
      				mat->ptr[next] = mat->ptr[i];
      				next = i;
			        i = (next % rows) * columns + next / rows;
			}		
		mat->ptr[next] = tmp;
		}
	}
    
}

/**
 * Square Matrix Multithread
 * @brief Multithreaded cycles transposer for MxN matrices
 * 
 */
void SquareMultithread(const unsigned rows, const unsigned columns, Mat *mat)
{
    counter = 0;
    omp_set_num_threads(omp_thread_count);
    #pragma omp parallel
    {
        //printf("Omp thread made!\n");
        SquareMTCyclesHelper(rows, columns, mat);
    }
    int temp = mat->m;
    mat->m = mat->n;
    mat->n = temp;
}

/*
    ========================================
    N x M Matrix Transposition
*/
/**
 * Square Matrix Cycles Helper
 * @brief TODO Add comments and explanation
 * 
 */
void rectangularMTCyclesHelper(const unsigned rows, const unsigned columns, Mat *mat)
{
    
	int holder = __atomic_fetch_add(&counter, 1, __ATOMIC_SEQ_CST);
    unsigned start_num = holder * ((rows * columns - 2)/omp_thread_count);
	unsigned array_size = start_num + (rows * columns - 2)/omp_thread_count;


	for(int start = start_num; start <= array_size; start++)
	{
		unsigned i = 1;
		unsigned next = (start % rows) * columns + start / rows;
		while(next > start)
		{
			i++;
			next = (next % rows) * columns + next / rows;
		}
		if (next >= start && i != 1)
		{
			const double tmp = mat->ptr[start];
			next = start;
			i = (next % rows) * columns + next / rows;
			while (i > start)
      			{
      				mat->ptr[next] = mat->ptr[i];
      				next = i;
			        i = (next % rows) * columns + next / rows;
			}		
		mat->ptr[next] = tmp;
		}
	}
    
}

void RectangularMultithread(const unsigned rows, const unsigned columns, Mat *mat)
{
    counter = 0;
    omp_set_num_threads(omp_thread_count);
    #pragma omp parallel
    {
        //printf("Omp thread made!\n");
        rectangularMTCyclesHelper(rows, columns, mat);
    }
    int temp = mat->m;
    mat->m = mat->n;
    mat->n = temp;
}
/**
 * Rectangular Matric Transposition with cycles
 * @brief Code for the professor
 * 
 */

void MTCycles(const unsigned rows, const unsigned columns, Mat* mat)
{
    unsigned start = 1;
    unsigned array_size = rows * columns - 2;
    while (start <= array_size)
    {
        unsigned i = 1;
        unsigned next = (start % rows) * columns + start / rows;

        while (next > start)
        {
            i++;
            next = (next % rows) * columns + next / rows;
        }
        if (next >= start && i != 1)
        {
            const double tmp = mat->ptr[start];
            next = start;
            i = (next % rows) * columns + next / rows;
            while (i > start)
            {
                mat->ptr[next] = mat->ptr[i];
                next = i;
                i = (next % rows) * columns + next / rows;
            }
            mat->ptr[next] = tmp;
        }
        start++;
    }
    int temp = mat->m;
    mat->m = mat->n;
    mat->n = temp;

}

/**
 * Rectangular Matric Transposition with cycles
 * @brief Updated cycles code with a bit of loop unfolding
 * 
 */
void Updated_MTCycles(const unsigned rows, const unsigned columns, Mat* mat)
{
    mat->m =columns;
    mat->n = rows;
    unsigned start = 1, array_size = rows * columns - 2;
    int loops = 0;
    unsigned i = 1;
    unsigned next;
    int min = array_size;
    //Loop unfolding, transposes cycles that are forsure never seen before
    while (start <= array_size && start < min)
    {
        const double tmp = mat->ptr[start];
        next = start;
        i = (next % rows) * columns + next / rows;
        while (i > start)
        {
            mat->ptr[next] = mat->ptr[i];
            next = i;
            i = (next % rows) * columns + next / rows;
            if(i < min && i != start){
                min = i;
            }
            loops++;
        }
        
        mat->ptr[next] = tmp;
        start++;
        loops++;
    }
    //start of the original code, looks for cycle then transposes 
    while (start <= array_size)
    {
        i = 1;
        next = (start % rows) * columns + start / rows;
        while (next > start )
        {
            i++;
            next = (next % rows) * columns + next / rows;
        }
        if (next >= start && i != 1 )
        {
            loops += i;
            const double tmp = mat->ptr[start];
            next = start;
            i = (next % rows) * columns + next / rows;
            while (i > start)
            {
                mat->ptr[next] = mat->ptr[i];
                next = i;
                i = (next % rows) * columns + next / rows;
            }
            mat->ptr[next] = tmp;
            //breaks the program early if seen all cycles
            if (loops == array_size - 2){
                return;
            }
        }
        start++;
    }
    return;
}

// Small, but not in place. slow (mostly)!
/**
 * Rectangular Matrix transposition
 * @brief multithreaded, unoptimized NxN matrix out-of-place transpose 
 * 
 */
void smallMulTranspose(Mat *mat, Mat *mat2){
    int N = mat->n;
    int M = mat->m;
    #pragma omp parallel for
    for(int n = 0; n<N*M; n++) {
        int i = n/N;
        int j = n%N;
        mat2->ptr[n] = mat->ptr[M*j + i];
    }
}

/**
 * Rectangular Matrix Cycles Helper
 * @brief Helper for transpose_block
 * 
 */
void transpose_scalar_block(int *A, int *B, const int M, const int N, const int block_size) {
    #pragma omp parallel for
    for(int i=0; i<block_size; i++) {
        for(int j=0; j<block_size; j++) {
            B[j*N + i] = A[i*M +j];
        }
    }
}

/**
 * Rectrangular Matrix Multithread
 * @brief optimized, multithreaded, cache friendly, out-of-place matrix transpose
 * 
 */
void transpose_block(Mat *A, Mat *B,  const int block_size) {
    const int M = A->n;
    const int N = A->m;
    #pragma omp parallel for
    for(int i=0; i<N; i+=block_size) {
        for(int j=0; j<M; j+=block_size) {
            transpose_scalar_block(&A->ptr[i*M +j], &B->ptr[j*N + i], M, N, block_size);
        }
    }

    B->m = M;
    B->n = N;
}


struct argument {
    Mat* A;
} argument;

int global_start;
//Multithreaded version of matrix transposition with cycles
void* helper(void* ptr){
    struct argument* arg = (struct argument*) ptr;
    Mat* A = arg->A;
    unsigned start = __atomic_fetch_add(&global_start, 1, __ATOMIC_SEQ_CST);
    int columns = A->n;
    int rows = A->m;
    unsigned array_size = rows * columns - 2;;
    while (start <= array_size)
    {
        unsigned i = 1;
        unsigned next = (start % rows) * columns + start / rows;
        while (next > start)
        {
            i++;
            next = (next % rows) * columns + next / rows;
        }
        if (next >= start && i != 1)
        {
            const double tmp = A->ptr[start];
            next = start;
            i = (next % rows) * columns + next / rows;
            while (i > start)
            {
                A->ptr[next] = A->ptr[i];
                next = i;
                i = (next % rows) * columns + next / rows;
            }
            A->ptr[next] = tmp;
        }
        start = __atomic_fetch_add(&global_start, 1, __ATOMIC_SEQ_CST);;
    }
    pthread_exit(NULL);
}

//Start the threads for inplace mulithreading
void InplaceMulti(Mat *A){
    int thread = omp_thread_count;
    global_start = 0;
    pthread_t threads[thread];
    struct argument arg;
    arg.A = A;
    for (int i = 0; i < thread; i++) {
        pthread_create(&threads[i], NULL, helper, (void*) &arg);
    }
    for (int i = 0; i < thread; i++)
        pthread_join(threads[i], NULL);
    int temp = A->m;
    A->m = A->n;
    A->n = temp;

}
