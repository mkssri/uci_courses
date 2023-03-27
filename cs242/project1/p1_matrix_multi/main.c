#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "multiply.h"

int main()
{
    int n[12] = {5, 10, 25, 50, 100, 250, 500, 1000, 1500,2000, 2500, 3000};
    // int n[8] = {5, 10, 25, 50, 100, 250, 500, 1000};

    double time_spent = 0;
    double end, begin = 0;
    size_t len_n = sizeof(n) / sizeof(int);


    for(int i = 0; i < len_n; i++){

        Mat mats[4];

        // Matrix initializations
        mat_init(&mats[0], n[i], n[i]);
        mat_init(&mats[1], n[i], n[i]);
        mat_init(&mats[2], n[i], n[i]);

        // Assinging matixes with randomn numbers
        randomize(&mats[0]);
        randomize(&mats[1]); 


        // Matrix initializations
        mat_init(&mats[3], n[i], n[i]);
        printf("%d x %d \n", n[i], n[i]);
        mat_copy(&mats[3], &mats[2]);

        /*
            single Threaded Matrix Multiplication
        */
        begin = clock();
        singleThreadedMulti(&mats[0], &mats[1], &mats[3]);
        end = clock();
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("single threaded time: %f seconds\n", time_spent);
        mat_copy(&mats[3], &mats[2]);

        /*
            single Threaded Matrix Multiplication with Cache
        */
        begin = clock();
        singleThreadedMuliCache(&mats[0], &mats[1], &mats[3]);
        end = clock();
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("single threaded cache friendly time: %f seconds\n", time_spent);
        mat_copy(&mats[3], &mats[2]);


        /*
            Multi Threaded Matrix Multiplication without Cache
        */
        begin = clock();
        multithreadedMulti(&mats[0], &mats[1], &mats[3], 4, 0);
        end = clock();
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("Multithreaded not cache friendly time: %f seconds\n", time_spent);
        mat_copy(&mats[3], &mats[2]);


        /*
            Multi Threaded Matrix Multiplication with Cache
        */
        begin = clock();
        multithreadedMulti(&mats[0], &mats[1], &mats[3], 4, 1);
        end = clock();
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("Multithreaded cache friendly time: %f seconds\n", time_spent);
        printf("\n");
    }
       
}
