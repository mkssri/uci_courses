#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>


#include "primes.h"//implementing

/*
typedef struct { //example structure
    int example;
    int e_g;
} Example_Structure;

static void example_helper_function(int n){
    // Functions defined with the modifier 'static' are only visible
    // to other functions in this file. They cannot be called from
    // outside (for example, from main.c). Use them to organize your
    // code. Remember that in C, you cannot use a function until you
    // declare it, so declare all your utility functions above the
    // point where you use them.
    //
    // Maintain the primes_xt functions as lean as possible
    // because we are measuring their speed. Unless you are debugging,
    // do not print anything on them, that consumes precious time.
    //
    // You may delete this example helper function and structure, and
    // write your own useful ones.

    Example_Structure es1;
    es1.example = 13;
    es1.e_g = 7;
    printf("n = %d\n", es1.example + es1.e_g + n);
    return;
}

*/

/*
Function to check if given number is Prime number or not -
it returns true if given number is prime else, it returns false

Following function checks if given number is divisible by any number
from 1 to sqrt(n), if it is divisible then it returns false(since number is not prime)
else it returns true(since number is prime)
*/
bool isNumPrimeCheck(int num) {
    if((num == 1) || (num != 2  && num%2 == 0)) {
        return false;
    }
    for(int x = 2; x <= (int)sqrt(num); x++) {
        if(num%x == 0) {
            return false;
        }
    }
    return true;
}

/*
SINGLE THREAD (SEQUENTIAL EXECUTION) SECTION

Following function starts from 1 and continues till max number(max) and checks if current number
is prime or not. if verbose = 0 it wont print prime number in output, where as if it is 1
it will print prime number in output terminal..below method is executed as single thread
*/

void primes_st(unsigned int max, unsigned int verb){
    //Put your code here.
    //example_helper_function(1000);

    for(int num = 1; num <= max; num++) {
        if((isNumPrimeCheck(num)) && (verb == 1)) {
            printf("%d\n", num);
        }
    }
    return;
}


/*
MULTI THREAD SECTION
*/


/*
Following is structure to store data, every thread accesses
the data from below structure.
*/
typedef struct dataForThread {
    int maxVal;
    int verboseInput;
    pthread_mutex_t *mutex;
    int numCounter;
}DataForThread_th;

/*
Following function does computations in each thread, it gets a number
and locks the common data  so other threads cannot access it, once it loads the
number succesfully in to currNum, it unlocks the common data(so other threads can access it). 
Next each thread will check if currNum is prime or not and it prints the number if verbose is 0.
*/
void *computations_in_thread_func(void *args) {

    int currNum=0;
    DataForThread_th *thData = (DataForThread_th *)args;
    while(currNum <= thData->maxVal) {

        // Lock the mutex
        pthread_mutex_lock(thData->mutex);
        // Incrementing the counter
        currNum = ++(thData->numCounter);
        // Unlock the mutex
        pthread_mutex_unlock(thData->mutex);

        // if current number is greater than the maximum value(input)
        // we can exit this therad. as this thread
        if(currNum > thData->maxVal) {
            pthread_exit(NULL);
        }
        else if((isNumPrimeCheck(currNum)) && (thData->verboseInput == 1)) {
            printf("%d\n", currNum);
        }
        
    }
    pthread_exit(NULL);
}

/*
Following function creates threads and does computations further merges child threads to the
main thread once its execution completes
*/
void primes_mt(unsigned int max, unsigned int threads,	\
	       unsigned int verb){
    //Put your code here.
    //example_helper_function(2000);
    pthread_t *lstOfThreads;
    lstOfThreads = (pthread_t *)malloc(sizeof(pthread_t) * threads);

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    DataForThread_th data_for_th;
    data_for_th.maxVal = max;
    data_for_th.verboseInput = verb;
    data_for_th.mutex = &mutex;
    data_for_th.numCounter = 1;

    /*
    Creating multiple threads from single main thread and doing commutations to check if a number is 
    prime or not
    */
    for(int x = 0; x<threads; x++) {
        pthread_create(lstOfThreads + x, NULL, computations_in_thread_func, (void *)&data_for_th);
    }

    /*
    Following helps to join all the threads to the main thread once thread 
    execution completes
    */
    for(int x = 0; x<threads; x++) {
        pthread_join(lstOfThreads[x], NULL);
    }
    return;
}