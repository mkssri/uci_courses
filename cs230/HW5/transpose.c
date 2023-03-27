
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "util.h"//implementing

typedef struct {
    pthread_mutex_t *pmtx;
    Mat *mat;
    int *i;
    int *j;
    int *grain;
} Args;

void do_transpose(int i, int j, int grain, Mat* mat) {
    int n=mat->n;
    for(;j<n;j++) {
        int idxa = i*n + j;
        int idxb = j*n + i;
        double temp = mat->ptr[idxa];

        mat->ptr[idxa] = mat->ptr[idxb];
        mat->ptr[idxb] = temp;
        grain = grain-1;

        if(grain <= 0) {
            break;
        }
    }
    i+=1;
    if(grain>0){
        for(; i<n; i++) {
            for(j=i+1; j<n;j++) {
                int idxa = i*n + j;
                int idxb = j*n + i;
                double temp = mat->ptr[idxa];
                mat->ptr[idxa] = mat->ptr[idxb];
                mat->ptr[idxb] = temp;
                grain = grain-1;
                if(grain <= 0) {
                    break;
                }
            }
            if(grain <= 0) {
                break;
            }
        }
    }
}

void* transpose(void *args) {
    Args* a=(Args*) args;
    int tempgr;
    int newi, newj;
    int tj,i;
    int curri, currj;
    Mat* mat=a->mat;
    int n,m;
    n=mat->n;
    m=mat->m;
    do{
        pthread_mutex_lock(a->pmtx);
        curri=*(a->i);
        currj=*(a->j);
        tj=currj;
        tempgr=*(a->grain);
        for(i=curri;i<m;i++){
            if(tempgr>= (n-tj)){
                tempgr= tempgr - (n-tj);
            }else{
                break;
            }
            tj=i+2;
        }
        newi=i;
        if(newi==curri){
            newj = currj+tempgr;
        }else{
            newj=i+tempgr+1;
        }
        *(a->i)=newi;
        *(a->j)=newj;
        pthread_mutex_unlock(a->pmtx);
        tempgr=*(a->grain);
        do_transpose(curri,currj,tempgr,mat);
        
    }while(newi <m && newj<n );
    
    pthread_exit(NULL);
}

void mat_sq_trans_mt(Mat *mat, unsigned int grain, unsigned int threads){
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    pthread_t mythreads[threads];
    int row = 0,column=1,i;
    int grains=grain;
    Args arg_thr={&mutex, mat, &row, &column,&grains};
    int rc = 0;

    for(i=0; i<threads; i++) {
        rc = pthread_create(&mythreads[i], NULL, &transpose,(void*) &arg_thr);
    }

    if(rc){
        printf("Error creating thread\n");
        exit(-1);
    }

    for(i=0;i<threads;i++){
        pthread_join(mythreads[i],NULL);
    }

    return;
}

void mat_sq_trans_st(Mat *mat){
    int i,j;
    int m,n,temp;
    m=mat->m;
    n=mat->n;
    for(i=0;i<m;i++) {
        for(j=i+1;j<n;j++) {
            temp=mat->ptr[n*i + j];
            mat->ptr[n*i+j]=mat->ptr[n*j+i];
            mat->ptr[n*j+i]=temp;
        }
    }
    return;
}
