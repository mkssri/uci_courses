#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "bitonic.h"
#include <omp.h>

#define omp_thread_count 4


/**
 * @brief Randomizes the arrays values 
 * 
 * @param size size of the array
 * @param a the array to sort
 */
void randomizeArray(int size, int *a){
    srand(time(0) * size);
    for(int i = 0; i < size; i++){
        a[i]= rand() % 100;
    }
    return;
}

/**
 * @brief Finds the next power based 2
 * 
 * @param n the current number
 * @return unsigned the next highest power
 */
unsigned nextPower(unsigned n)
{
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return ++n;
}

/**
 * @brief Print the array
 * 
 * @param size size of the array
 * @param a the array to print 
 */
void printArray(int size, int *a){
    for(int i = 0; i < size; i++){
        printf("  %2.1d ", a[i]);
    }
    printf("\n");
    return;
}

/**
 * @brief Copies the array
 * 
 * @param size size of the array
 * @param a the input arrray 
 * @param b the output array
 */
void copyArray(int size, int *a, int *b){
    for(int i = 0; i < size; i++){
        b[i] = a[i];
    }
    return;
}

/**
 * @brief Checks if two arrays are equal
 * 
 * @param size size of the array
 * @param a the first input array
 * @param b the second input array
 */
void arrayEqual(int size, int *a, int *b){
    for(int i = 0; i < size; i++){
        if(b[i] != a[i]){
            printf("Not Equal\n");
            return;
        }
    }
    printf("Equal\n");
    return;
}

/**
 * @brief Checks if an array of the input size can be sorted 
 * by bitonic sort
 * 
 * @param size size of the array
 * @return int 1 if false 0 if true
 */
int sortable(int size){
    double a = log(size)/log(2.0);
    if( a  != (int) a){
        return 1;
    }
    return 0;
}

/**
 * @brief Checks if an array is sorted smallest to largest.=
 * 
 * @param size size of the array
 * @param a the array to be sorted
 */
void sortChecker(int size, int *a){
    for(int i = 1; i < size; i++){
        if(a[i] < a[i-1]){
            printf("Array Non Sorted: %d was before %d", a[i-1], a[i]);
            return;
        }
    }
    printf("Array sorted!\n");
}

/**
 * @brief Calculates the number of segments
 * 
 * @param size the size of the total array
 * @return int the number of segments
 */
int numSegments(int size){
    int result = pow(2, (int) (log(size)/log(2.0)) -3);
   // printf("%d %d \n", size, result);
    if(result == 1 ) return 2;
    return result;
}

/**
 * @brief finds the starting index with number of segments for size
 * 
 * @param size the size of the array
 * @param segment the number of segments
 * @return the starting index for a segment
 */
int startingIndex(int size, int segment){
    if(numSegments(size) == segment){
        return size;
    }else{
        return ceil(((double) size/ (double) numSegments(size)) * (segment + 0.0)) ;
    }
}


/**
 * @brief a merge method for two segments. Using insertion sort
 * 
 * @param i the index of the first segment
 * @param l the index of the second segement
 * @param size size of the array
 * @param arr the array to sort
 * @param flag 0 if sorting smallest to larges, 1 if sorting largest to smallest
 */
void merge(int i, int l,int size, int *arr, int flag){
    //Indexing the segments
    int starti = startingIndex(size, i);
    int endi = startingIndex(size, i+1);
    int startl =startingIndex(size, l);
    int endl = startingIndex(size, l+1);
    int a, b, index;
    int tempA[16];
    b = 0;
    for(a =  starti; a < endl;){
        tempA[b] = arr[a];
        b++;
        a++;
        if(a == endi){
            a = startl;
        }
    }
    //simple insertion sort with modification of segments not next to each other

    for (a = 1; a < 16; a++)
    {
        index = tempA[a];
        b = a - 1;
        while (b >= 0 && tempA[b] > index && flag == 0)
        {
            tempA[b + 1] = tempA[b];
            b = b - 1;
        }
        while (b >= 0 && tempA[b] < index && flag == 1)
        {
            tempA[b + 1] = tempA[b];
            b = b - 1;
        }
        tempA[b + 1] = index;
    }
    b=0;
    for(a =  starti; a < endl;){
        arr[a] = tempA[b];
        b++;
        a++;
        if(a == endi){
            a = startl;
        }
    }
}

/**
 * @brief Bitonic sort by segements. Must be 2^n size to sort can add padding. 
 * 
 * @param size Size of the array
 * @param a the array to be sorted
 */
void sortBySegments(int size, int *a){
    int segments = numSegments(size);
    //#pragma omp parallel for
    for (int k = 2; k <= segments; k *= 2){ // k is doubled every iteration
        for (int j = k/2; j > 0; j /= 2){ // j is halved at every iteration, with truncation of fractional parts
            for (int i = 0; i < segments; i++){
                int l =i ^ j; // in C-like languages this is "i ^ j"
                if (l > i){
                    if ((i&k)==0) {
                        //down min to the top and max bottom
                        merge(i,l,size, a, 0);
                    }
                    if ((i&k)!=0){
                        //up min to bottom max to top
                        merge(i,l,size, a, 1);
                    }
                }
            }
        }
    }
}

/**
 * @brief Bitonic sort by elements must be 2^n size to be sorted
 * 
 * @param size size of the array
 * @param a the array to be sorted
 */
void sortByElements(int size, int *a){
    for (int k = 2; k <= size; k *= 2){ // k is doubled every iteration
        for (int j = k/2; j > 0; j /= 2){ // j is halved at every iteration, with truncation of fractional parts
            for (int i = 0; i < size; i++){
                int l =i ^ j; // in C-like languages this is "i ^ j"
                if (l > i){
                    if ((i&k)==0 && a[i] > a[l]) {
                        //down min to the top and max bottom
                        int temp = a[i];
                        a[i] = a[l];
                        a[l] = temp;
                    }
                    if ((i&k)!=0 && a[i] < a[l]){
                        //up min to bottom max to top
                        int temp = a[i];
                        a[i] = a[l];
                        a[l] = temp;
                    }
                }
            }
        }
    }                        
}

/**
 * @brief Mulithreaded Bitonic sort by segments must be input size of 2^n to be sorted. 
 * This can be done by adding padding to the end of the array
 * 
 * @param size size of the input array
 * @param a the array to sort
 */
void sortBySegmentsMulti(int size, int *a){
    int segments = numSegments(size);
    for (int k = 2; k <= segments; k *= 2){ // k is doubled every iteration
        for (int j = k/2; j > 0; j /= 2){ // j is halved at every iteration, with truncation of fractional parts
           #pragma omp parallel for
            for (int i = 0; i < segments; i++){
                int l =i ^ j; // in C-like languages this is "i ^ j"
                if (l > i){
                    if ((i&k)==0) {
                        //down min to the top and max bottom
                        merge(i,l,size, a, 0);
                    }
                    if ((i&k)!=0){
                        //up min to bottom max to top
                        merge(i,l,size, a, 1);
                    }
                }
            }
        }
    }
}


/**
 * @brief Bitonic sort by elements using mulithreading must be 2^n to be sorted
 * 
 * @param size size of the array to be sorted
 * @param a the array
 */
void sortByElementsMulti(int size, int *a){
    for (int k = 2; k <= size; k *= 2){ // k is doubled every iteration
        for (int j = k/2; j > 0; j /= 2){ // j is halved at every iteration, with truncation of fractional parts
            #pragma omp parallel for
            for (int i = 0; i < size; i++){
                int l =i ^ j; // in C-like languages this is "i ^ j"
                if (l > i){
                    if ((i&k)==0 && a[i] > a[l]) {
                        //down min to the top and max bottom
                        int temp = a[i];
                        a[i] = a[l];
                        a[l] = temp;
                    }
                    if ((i&k)!=0 && a[i] < a[l]){
                        //up min to bottom max to top
                        int temp = a[i];
                        a[i] = a[l];
                        a[l] = temp;
                    }
                }
            }
        }
    }                        
}


/**
 * @brief A simple selection sort
 * 
 * @param n the size of the array
 * @param arr array to be sorted
 */
void selectionSort(int n, int *arr)
{
    int i, key, j;
    for (i = 1; i < n; i++)
    {
        key = arr[i];
        j = i - 1;
 
        /* Move elements of arr[0..i-1], that are
        greater than key, to one position ahead
        of their current position */
        while (j >= 0 && arr[j] > key)
        {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}
