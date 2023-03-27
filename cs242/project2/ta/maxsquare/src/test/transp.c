#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "transpose.h" //testing
#include "measure.h"
#include "vector.h"
#include "util.h"

extern size_t EXCH_MIN_GRAIN;
extern size_t SQTR_MIN_GRAIN;
static int wrong_transpose(Mat trans, Mat orig, size_t verb);

int main(int argc, char **argv){
    enum opts{M,N,CHK,EXP,T,VERB,SEED,DET,MIN_EX,MIN_SQ};
    struct arg opt[]={
        { 0, 0, 1, "m", "Maximum height of the matrix"},
        { 0, 0, 1, "n", "Maximum width of the matrix"},
        { 0, 0, 0, "chk", "Check the correctness of the operation's result"},
        { 1, 0, 0, "exp", "Number o experiments to run"},
        { 1, 0, 0, "thr", "Threads to use"},
        { 0, 0, 0, "verb", "Verbose? {0, 1, 2, 3}"},
        {(unsigned int)time(NULL), 0, 0, "seed", "Seed for random generator"},
        { 0, 0, 0, "det", "Deterministic run (invalidates seed). "
          "Use m and n as given. {0, 1}"},
        { 1, 0, 0, "mex", "Minimum work-per-thread to multi-thread exchange"},
        { 1, 0, 0, "msq", "Minimum work-per-thread to multi-thread square "
          "transpose"}};
    getargs("In-Place Transposition of a rectangular matrix of size m*n "
            "doubles", argv, argc, opt, 10);
    srand((unsigned)opt[SEED].v);
    EXCH_MIN_GRAIN = opt[MIN_EX].v;
    SQTR_MIN_GRAIN = opt[MIN_SQ].v;
    
    // print header
    if(opt[VERB].v){
        printf("max m    : %llu\n", opt[M].v);
        printf("max n    : %llu\n", opt[N].v);
        printf("exp      : %llu\n", opt[EXP].v);
        printf("threads  : %llu\n", opt[T].v);
        printf("run type : ");
        if(opt[DET].v)
            printf("Deterministic.\n");
        else
            printf("Rand. seed=%lld\n",opt[SEED].v);
        printf("\n");
    }

    // experiment's preamble
    if(1 < opt[VERB].v){
        printf("%4s %12s %12s ", "#", "m", "n");
        if(opt[CHK].v)
            printf("%12s ", "result");
        printf("%16s\n", "time");
    }else if(opt[VERB].v)
        printf("Time [ns]: ");
    
    // run experiments
    Mat m1, mctrl;
    int i;
    size_t mp, np, tmp_dim;
    int64_t t;
    msr_init(NULL, 0, NULL, 0);
    for(i=0; i<opt[EXP].v; i++){
        // set matrix size for this experiment
        if(opt[DET].s){
            mp = opt[M].v;
            np = opt[N].v;
        }else{
            size_t above=10; // random between above% and 100%
            mp = above * opt[M].v + rand()%((100-above)*opt[M].v+1);
            mp /= 100;
            np = above * opt[N].v + rand()%((100-above)*opt[N].v+1);
            np /= 100;
        }
        
        //create matrices
        mat_create(&m1, mp, np);
        if(opt[CHK].v)
            mat_create(&mctrl, mp, np);
        mat_fill_coor(&m1);
        if(opt[CHK].v){
            mat_copy(&mctrl, &m1);
        }
        
        // print experiment parameters
        if(1 < opt[VERB].v){
            printf("%4d %12lu %12lu ", i+1, mp, np);
            fflush(stdout);
        }
        
        // measure
        t = nano_stopwatch();
        msr_record();
        transpose(&m1.ptr, m1.m, m1.n, opt[T].v);
        msr_pause();
        t = nano_stopwatch() - t;
        //flip dimensions
        tmp_dim = m1.m;
        m1.m = m1.n;
        m1.n = tmp_dim;
        
        // if requested, check results
        if(opt[CHK].v && wrong_transpose(m1, mctrl, opt[VERB].v)){
            if(1 < opt[VERB].v)
                printf("%12s \n", "failed");
            printf("Error: Matrix is not correctly transposed.\n");
            printf("Original:\n");
            mat_print(&mctrl);
            printf("\nTransposed:\n");
            mat_print(&m1);
            exit(-1);
        }

        // print "passed" and time taken
        if(1 < opt[VERB].v){
            if(opt[CHK].v)
                printf("%12s ", "passed");
            printf("%12lu[ns]\n", t);
            if(2 < opt[VERB].v){
                printf("Original:\n");
                mat_print(&mctrl);
                printf("Transposed:\n");
                mat_print(&m1);
            }
        }else if(opt[VERB].v){
            printf("%16lu", t);
            if(i+1 < opt[EXP].v)
                printf(",");
            else
                printf("\n");
        }

        // destroy matrices
        mat_destroy(&m1);
        if(opt[CHK].v)
            mat_destroy(&mctrl);
    }

    // finish computing average and print results
    struct read_sformat avg_rsf;
    avg_rsf = msr_get();
    divide_rsf(&avg_rsf,opt[EXP].v);
    if(opt[VERB].v)
        printf("\nAverage Counters:\n");
    msr_print_results(&avg_rsf, opt[VERB].v);
    return 0;
}

static int wrong_transpose(Mat trans, Mat orig, size_t verb){
    if(orig.m == 0 || orig.n == 0)
        return 0;
    for(size_t m=0; m<orig.m; m++){
        for(size_t n=0; n<orig.n; n++){
            if(orig.ptr[m*orig.n + n] != trans.ptr[n*orig.m + m])
                return 1;
        }
    }
    return 0;
}
