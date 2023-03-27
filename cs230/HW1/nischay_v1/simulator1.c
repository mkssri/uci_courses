/* SUBMIT ONLY THIS FILE */
/* NAME: ....... */
/* UCI ID: .......*/

// only include standard libraries.
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define MAX_CYCLES (1000 * 1000)
#define EPSILON (0.001f)

double sqrt(double);
double cos(double);
double log(double);
double pow(double,double);

double rand_gen() {
   // return a uniformly distributed random value
   return ( (double)(rand()))/( (double)(RAND_MAX) + 1. );
}

double uniform_random(int rangeLow, int rangeHigh) {
    double myRand = rand()/(1.0 + RAND_MAX); 
    int range = rangeHigh - rangeLow + 1;
    int myRand_scaled = (myRand * range) + rangeLow;
    return myRand_scaled;

    // double number = (double)(rand()%memnum);
    // return number;
}

int normalRandom(int memnum, double mean) {
   // return a normally distributed random value
    double v1=rand_gen();
    double v2=rand_gen();
//    double number =cos(2*3.14*v2)*sqrt(-2.*log(v1));
    double randN = cos(2 * 3.14 * v2)*sqrt(-2.0 *log(v1));
    double sigma = memnum/6.0;
    int x = abs(randN*sigma+mean);
    return x%memnum;
}

void simulate(double *avg_wait, int avg_wait_l, int procs, char dist){

    srand(time(NULL));

    int i;
    for(i=0; i<avg_wait_l; i++){
        avg_wait[i] = 0;
    }

    int mem_size = 1;
    while(mem_size <= avg_wait_l) {
        
        int *request = (int*) calloc(procs, sizeof(int));
        
        int *access_granted = (int*) calloc(procs, sizeof(int));

        int *access_denials = (int*) calloc(procs, sizeof(int));
        
        int *priority = (int*) calloc(procs, sizeof(int));
        
        int *memory = (int*) calloc(mem_size, sizeof(int));
        
        double *mean = (double*) calloc(procs, sizeof(double));

        bool *visited = (bool*) calloc(procs, sizeof(bool));
        
        if(dist=='n'){
            for(int p=0;p<procs;p++){
                mean[p] = (double) uniform_random(0, mem_size-1);
            }
        }
        
        int cycle = 1, max_priority = 0, flag = 0, num_less_than_epsilon = 0;
        double prev_avg = 0.0, curr_avg = 0.0;
        
        for(cycle = 1; cycle < MAX_CYCLES; cycle++) {
            
            /* Reset visited array */
            for(i=0; i<procs; i++) {
                visited[i] = false;
            }
         
            /* Reset memory */
            for(i=0; i<mem_size; i++){
                memory[i] = 0;
            }
        
            int curr_priority = max_priority;
            while(curr_priority >= 0) {
        
                for(i=0; i<procs; i++) {
                    if(priority[i] == curr_priority && !visited[i]) {

                        /* The processor whose memory was served in the last cycle */
                        if(priority[i] == 0){
                            int memory_index;
                            if(dist=='u'){
                                memory_index = uniform_random(0, mem_size-1);
                            }else{
                                memory_index = normalRandom(mem_size,mean[i]);
                            }
                            request[i] = memory_index;
                        }

                        /* Memory is free */
                        if(memory[request[i]] == 0) {
                            // request[i] = -1;
                            visited[i] = true;
                            priority[i] = 0;
                            memory[request[i]] = 1;
                            access_granted[i]++;
                        }
                        /* Memory is not free */
                        else {
                            access_denials[i]++;
                            priority[i]++;
                            visited[i] = true;
                            if(priority[i] > max_priority){
                                max_priority = priority[i];
                            }
                        }
                    }
                }        
                curr_priority--;
            }
        
            if(flag == 0) {
                i = 0;
                while(i<procs) {
                    if(access_denials[i] == cycle)
                        break;
                    i++;
                }
            }
        
            if(i == procs)
                flag = 1;
            
            if(flag == 1) {
                double sum = 0.0;
                for(i=0; i<procs; i++) {
                    sum += (cycle/(cycle - access_denials[i]) * 1.0);
                }
        
                prev_avg = curr_avg;
                curr_avg = sum/procs/1.0;
        
                double check = 1.0 - (prev_avg/curr_avg);
                if(prev_avg != 0 && abs(check) < EPSILON) {
                    if(num_less_than_epsilon >= 10){
                        break;
                    }
                    num_less_than_epsilon++;
                }
                else {
                    num_less_than_epsilon = 0;
                }
        
            }
        }

        avg_wait[mem_size-1] = curr_avg;

        free(request);
        free(priority);
        free(access_granted);
        free(access_denials);
        free(memory);
        free(mean);
        free(visited);
        mem_size++;
    }
    
    // printf("procs: %d\ndist: %c\n", procs, dist);
    
}

