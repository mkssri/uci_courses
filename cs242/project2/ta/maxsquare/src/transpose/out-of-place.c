#include "transpose.h" //implements
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

size_t EXCH_MIN_GRAIN; // unused here
size_t SQTR_MIN_GRAIN; // unused here


struct th_d{
    size_t off;
    size_t size;
};

static double *old_mat, *new_mat;
static size_t m,n;

static void *transp_part(void *arg){
    /* printf("\n    transp_part()\n"); */
    size_t Boff = ((struct th_d*)arg)->off;
    size_t size = ((struct th_d*)arg)->size;
    size_t iA = (Boff % m) * n + (Boff / m);
    size_t iB = Boff;
    size_t tot = m*n;
    for(size_t t=0; t<size; t++){
        new_mat[iB] = old_mat[iA];
        iB += 1;
        if(iA+n < tot)
            iA += n;
        else
            iA = iA + n - tot + 1;
    }
    return NULL;
}

void transpose(double **mat, size_t mat_rows, size_t mat_cols, size_t thr){
    if(mat_rows < 1 || mat_cols < 1 )
        return;
    old_mat = *mat;
    new_mat = (double*) calloc(mat_cols*mat_rows, sizeof(double));
    if(!new_mat){
        printf("Couldn't allocate memory for new matrix");
        exit(-1);
    }
    if(thr < 2){
        size_t j_row; // strenght reduction of j*mat_rows
        size_t i_col; // strenght reduction of i*mat_cols
        j_row = 0;
        for(size_t j=0; j<mat_cols; j++){
            i_col = 0;
            for(size_t i=0; i<mat_rows; i++){
                new_mat[j_row+i] = (*mat)[i_col+j];
                i_col += mat_cols;
            }
            j_row += mat_rows;
        }
    }else{
        int t;
        if(mat_rows * mat_cols < thr)
            thr = mat_rows * mat_cols;
        m = mat_rows;
        n = mat_cols;
        // set part size
        struct th_d data[thr];
        size_t part = mat_rows*mat_cols/thr;
        size_t rest = mat_rows*mat_cols%thr;
        for(t=0; t<thr; t++){
            if(rest){
                data[t].size = part+1;
                rest -=1;
            }else
                data[t].size = part;
        }        
        //launch thr-1 new threads, and join.
        pthread_t thrs[thr];
        size_t off = 0;
        for(t=0; t<thr-1; t++){
            data[t].off = off;
            off += data[t].size;
            pthread_create(&thrs[t], NULL, &transp_part, (void*)&data[t]);
        }
        data[thr-1].off = off;
        transp_part((void*)&data[thr-1]); // Current thread runs last chunk
        for(t=0; t<thr-1; t++)
            pthread_join(thrs[t], NULL);
    }
    free(*mat);
    *mat = new_mat;
}        
