#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "transpose.h"

int main()
{	

    struct timespec start, end;
    double time_taken;

    /*
    ========================================
    N X N
    */

    // int n[1] = {1000};
    int n[12] = {50, 100, 250, 500, 1000, 1500,2000, 2500, 3000, 5000, 10000, 15000};
    size_t len_n = sizeof(n) / sizeof(int);

    int i = 0;
    for(int i = 0; i < len_n; i++){


        int row = n[i], column = n[i];
        printf("Matrix(row*col) - %d*%d\n", row, column);
        /*
            Matrix Transpose(Square) w/o integer multiplication (Sequential)
        */
        Mat squareMats[5];

        mat_init(&squareMats[0],row, column);
        mat_fill(&squareMats[0]);
        mat_init(&squareMats[1],row, column);
        mat_fill(&squareMats[1]);
        mat_init(&squareMats[2],row, column);
        mat_fill(&squareMats[2]);
        mat_init(&squareMats[3],row, column);
        mat_init(&squareMats[4],row, column);
        mat_fill(&squareMats[4]);

        //Naive matrix transposition
        clock_gettime(CLOCK_MONOTONIC, &start);
        naive_mat_transpose(&squareMats[0]);
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_taken = (end.tv_sec - start.tv_sec) * 1e9;
        time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
        printf("Naive Matix Transpose(Sequential) Exec time: %f seconds\n", time_taken);

        //sequential matrix transposition w/ multiplication or division
        clock_gettime(CLOCK_MONOTONIC, &start);
        mat_transpose(&squareMats[1]);
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_taken = (end.tv_sec - start.tv_sec) * 1e9;
        time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
        printf("Matix Transpose(Sequential) Exec time: %f seconds\n", time_taken);

        //multithreaded matrix transposition - unoptimized mulithreaded out-of-place transpose
        clock_gettime(CLOCK_MONOTONIC, &start);
        smallMulTranspose(&squareMats[2], &squareMats[3]);
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_taken = (end.tv_sec - start.tv_sec) * 1e9;
        time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
        printf("Matix Transpose(Multithreaded) unoptimized out-of-place Exec time: %f seconds\n", time_taken);

         //multithreaded matrix transposition - inplace transpose
        clock_gettime(CLOCK_MONOTONIC, &start);
        SquareMultithread(row, column, &squareMats[4] );
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_taken = (end.tv_sec - start.tv_sec) * 1e9;
        time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
        printf("Matix Transpose(Multithreaded) inplace Exec time: %f seconds\n", time_taken);

        printf("\nTo Test Accuracy \n");
        printf("Naive vs sequential mat transpose: ");
        mat_equal(&squareMats[0], &squareMats[1]);
        printf("Naive vs multithreaded mat transpose out of place: ");
        mat_equal(&squareMats[0], &squareMats[3]);
        printf("Naive vs multithreaded mat transpose in-place: ");
        mat_equal(&squareMats[0], &squareMats[4]);
        /* mat_print(&squareMats[1]);
            printf("\n");
            mat_print(&squareMats[2]);*/
        printf("\n");
        printf("\n");
    }

    /*
    ========================================
    N X M Rectangular Matrix Transposition
    */
   
    // int rows[1] = {10000};
    // int columns[1] = {7000};

    int rows[14] = {64, 112, 272, 496, 864, 1008, 3008, 5008, 7504, 10000, 12512, 15008, 17504, 20000};
    int columns[14] = {48, 80, 256, 480, 896, 1104, 2896, 4912, 7488, 10496, 12400, 14912, 16992, 19008};

    len_n = sizeof(rows) / sizeof(int);

    for(int i = 0; i < len_n; i++){

        int row = rows[i], column = columns[i];

        printf("Matrix(row*col) - %d*%d\n", rows[i], columns[i]);

        Mat rectMats[6];
        mat_init(&rectMats[0],row, column);
        mat_fill(&rectMats[0]);
        mat_init(&rectMats[1],row, column);
        mat_fill(&rectMats[1]);
        mat_init(&rectMats[2],row, column);
        mat_fill(&rectMats[2]);
        mat_init(&rectMats[3],row, column);
        mat_fill(&rectMats[3]);
        mat_init(&rectMats[4],row, column);
        

        //naive matrix transposition by cycles
        clock_gettime(CLOCK_MONOTONIC, &start);
        MTCycles(row, column, &rectMats[0]);
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_taken = (end.tv_sec - start.tv_sec) * 1e9;
        time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
        printf("Matix Transpose(Using cycles) Exec time: %f seconds\n", time_taken);

        //Updated matrix transposition by cycles
        clock_gettime(CLOCK_MONOTONIC, &start);
        Updated_MTCycles(row, column, &rectMats[1]);
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_taken = (end.tv_sec - start.tv_sec) * 1e9;
        time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
        printf("Updated Matix Transpose(Using cycles) Exec time: %f seconds\n", time_taken);


        //Multithreaded matrix(mxn) Transposition - Inplace Multi-Threaded
        clock_gettime(CLOCK_MONOTONIC, &start);
        RectangularMultithread(row, column, &rectMats[2]);
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_taken = (end.tv_sec - start.tv_sec) * 1e9;
        time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
        printf("Matix Transpose in-place(Using Multithreaded) unoptimized Exec time: %f seconds\n", time_taken);
    
        //Multithreaded Transposition by blocks (optimized)
        clock_gettime(CLOCK_MONOTONIC, &start);
        transpose_block(&rectMats[3], &rectMats[4], 16);
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_taken = (end.tv_sec - start.tv_sec) * 1e9;
        time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
        printf("Matix Transpose out-of-place (Multithreaded, cache-friendly) by blocks Exec time: %f seconds\n", time_taken);

        //Multithreaded Transposition In Place using cycles
        mat_init(&rectMats[5],row, column);
        mat_fill(&rectMats[5]);
        clock_gettime(CLOCK_MONOTONIC, &start);
        InplaceMulti(&rectMats[5]);
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_taken = (end.tv_sec - start.tv_sec) * 1e9;
        time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
        printf("Inplace Matrix Transposition w/ Pthreads Exec time: %f seconds\n", time_taken);


        printf("\nTo Test Accuracy \n");
        printf("Naive vs update mat transpose w/ cycles: ");
        mat_equal(&rectMats[0], &rectMats[1]);
        printf("Naive vs multithreaded mat transpose w/ omp(in-place): ");
        mat_equal(&rectMats[0], &rectMats[2]);
        printf("Naive vs multithreaded mat transpose w/ blocks(out-of-place): ");
        mat_equal(&rectMats[0], &rectMats[4]);
        printf("Naive vs  multithreaded mat transpose w/ pthreads (in-place): ");
        mat_equal(&rectMats[0], &rectMats[5]);
        printf("\n\n");



    
    }


}
