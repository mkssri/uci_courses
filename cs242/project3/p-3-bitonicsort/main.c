#include <stdio.h>
#include "bitonic.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define INT_MAX 2147483647

int main()
{
    struct timespec start, end;
    double time_taken;
    //int input_size = 1000000;
    printf("+---------+------------------+-----------------------+------------------+------------------------+\n");
    printf("|    Size | Sort By Elements | Sort by Elements Multi| Sort by Segments | Sort By Segments Multi |\n");
    printf("+---------+------------------+-----------------------+------------------+------------------------+\n");
    for(int i = 10; i < 22; i++)
    {
        int input_size = pow(2,i);
        printf("| %7d |", input_size);
        int size =  nextPower(input_size);
        sortable(size);
        
        int *a= new int[size];
        randomizeArray(size, a);

        for(int i = input_size; i < size; i++){
            a[i] = INT_MAX;
        }
        int *b= new int[size];
        int *c= new int[size];
        int *d= new int[size];
        copyArray(size, a, b);
        copyArray(size, a, c);
        copyArray(size, a, d);


        //---SINGLE THREAD ELEMENT---//
        clock_gettime(CLOCK_MONOTONIC, &start);
        sortByElements(size,a);
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_taken = (end.tv_sec - start.tv_sec) * 1e9;
        time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
        //sortChecker(size, a);
        printf("         %8f |", time_taken);


        //---MULTI THREAD ELEMENT---//
        clock_gettime(CLOCK_MONOTONIC, &start);
        sortByElementsMulti(size,b);
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_taken = (end.tv_sec - start.tv_sec) * 1e9;
        time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
        //sortChecker(size, b);
        printf("              %8f |", time_taken);


        //---SINGLE THREAD SEGMENT---//
        clock_gettime(CLOCK_MONOTONIC, &start);
        sortBySegments(size,c);
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_taken = (end.tv_sec - start.tv_sec) * 1e9;
        time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
        //sortChecker(size, c);
        printf("   %14f |", time_taken);


        //---MULTI THREAD SEGMENT---//
        clock_gettime(CLOCK_MONOTONIC, &start);
        sortBySegmentsMulti(size,d);
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_taken = (end.tv_sec - start.tv_sec) * 1e9;
        time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
        //sortChecker(size, d);
        printf("         %14f |\n", time_taken);


        free(a);
        free(b);
        free(c);
        free(d);
    }
    printf("+---------+------------------+-----------------------+------------------+------------------------+\n");
    return 0;
}

