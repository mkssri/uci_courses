#include "square-transp.h" //implements
#include <pthread.h>

size_t SQTR_MIN_GRAIN = 500;

static double *MAT; // pointer to the square matrix.
static size_t MAT_SIZE; // size of one side of the square matrix.
static size_t THREADS; // number of threads to use.
static size_t TOT_WORKLOAD; // total workload (size of lower triangular)

/**
 ** Packaging of all necesary info for thread to work.
 **/
struct thread_data{
    size_t offset;
    size_t work;
};

static void transpose_square_st(){
    size_t begin_line, start_point, n_edge, i_row = 1, sym_element;
    double tmp;
    begin_line = start_point = 0;
    while(i_row < MAT_SIZE){   	
        sym_element = i_row;
        start_point = begin_line = begin_line + MAT_SIZE;
        n_edge = i_row++;		 	
        do{		   
            tmp = MAT[sym_element];
            MAT[sym_element] = MAT[start_point];
            MAT[start_point] = tmp;	    
            sym_element = sym_element + MAT_SIZE;
            start_point++;
        } while(--n_edge > 0);		
    }
}

static void *MTSquareLT(void *arg){
    size_t start_point = ((struct thread_data*) arg)->offset;
    size_t ilength = ((struct thread_data*) arg)->work;

    size_t sym_element, n_edge, begin_line, i_row;
    double tmp;

    i_row = (start_point / MAT_SIZE);
    begin_line = i_row * MAT_SIZE;
    sym_element = ((start_point - begin_line) * MAT_SIZE) + i_row;
    n_edge = (i_row * (MAT_SIZE + 1)) - start_point;
    while(ilength > 0){
        n_edge = n_edge < ilength ? n_edge : ilength;
        ilength = ilength - n_edge;		
        do{
            tmp = MAT[sym_element];
            MAT[sym_element] = MAT[start_point];
            MAT[start_point] = tmp;	    
            sym_element = sym_element + MAT_SIZE;
            start_point++;	    
        } while (--n_edge > 0);
        start_point = begin_line = begin_line + MAT_SIZE;
        n_edge = sym_element = ++i_row;
    }
    pthread_exit(NULL);
}

static size_t next_start(size_t ploff, size_t pwork){
    //args: lineal offset of previous thread, and its work
    size_t i, j; //coordenates i and j
    size_t cpw = 0; // cumulated previos work
    i = ploff/MAT_SIZE;
    j = ploff%MAT_SIZE;
    while(i < MAT_SIZE){
        if(cpw + i - j <= pwork){
            //cpw increments the whole line.
            cpw += i - j; // add the remainder of this line
            i++; //jump to beginning of next line.
            j = 0;
        }else{
            j += pwork - cpw; //adds just part of this line
            break;
        }
    }
    return i * MAT_SIZE + j;
}

static void transpose_square_mt(void){
    size_t i, thread_work, rem_thr_wl, useless_threads=0;
    pthread_t thr[THREADS];
    struct thread_data tdata[THREADS];

    // Distribute the work among threads
    thread_work = TOT_WORKLOAD / THREADS; // even distribution of the work
    rem_thr_wl = TOT_WORKLOAD % THREADS; // remaining work
    // with the +1, distribute the remaining work among the first rem_thr_wl threads.
    for(i=0; i<rem_thr_wl; i++)
        tdata[i].work = thread_work+1;
    // to the rest of the threads, the normal work (if there is any).
    if(thread_work == 0)
        useless_threads = THREADS - rem_thr_wl;
    else
        for(; i<THREADS; i++)
            tdata[i].work = thread_work;

    // Computes the starting point of each thread based on the
    // starting point and work of the previous one.
    tdata[0].offset = MAT_SIZE; //first thread statrs at M[1,0]
    for(i=1; i < THREADS-useless_threads; i++)
        tdata[i].offset = next_start(tdata[i-1].offset, tdata[i-1].work);
    
    // Create and join the threads that perform the transposition.
    for(i = 0; i < THREADS-useless_threads; ++i)
        pthread_create(&thr[i], NULL, &MTSquareLT, &tdata[i]);
    for (i = 0; i < THREADS-useless_threads; ++i)
        pthread_join(thr[i], NULL);
    return;
}

void *transpose_square(double *mat, size_t rows, size_t thr){
    MAT = mat;
    MAT_SIZE = rows;
    TOT_WORKLOAD = rows*(rows-1)/2;
    
    if(TOT_WORKLOAD/thr < SQTR_MIN_GRAIN){
        THREADS = (TOT_WORKLOAD+SQTR_MIN_GRAIN-1)/SQTR_MIN_GRAIN;
    }else{
        THREADS = thr;
    }
    if(1 < THREADS){
        transpose_square_mt();
    }else{
        transpose_square_st();
    }
    return NULL;
}
