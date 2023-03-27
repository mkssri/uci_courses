/* SUBMIT ONLY THIS FILE */

/*============================
STUDENT-1 
NAME: Murali Krishna Sai Sriram
UCI ID: 29405372

STUDENT-2 
NAME: Yagyawalcya Sen
UCI ID: 42124775
============================*/


// only include standard libraries.
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define TOTAL_NO_OF_CYCLES (1000000)
#define EPSILON (0.0000001)
#define DIFF_IN_ACC_TIME_WITH_IN_LIMIT_VAL (10)

/* 
Following funtion returns a random value which is uniformly distributed
*/
double randomNumberGeneratorUniform() {
   return ((double)(rand()))/(1.0 + (double)(RAND_MAX));
}

int* generateMeanProcs(int procs, int mem_size) {

    int *mean_proc = (int *)malloc(sizeof(int) * procs);
    for(int i = 0; i < procs; i++) {
        mean_proc[i] = randomNumberGeneratorUniform() * mem_size;
    }
    return mean_proc;
}

/*
Following funtion returns a random value which is normally distributed
*/
int randomNumberGeneratorNormal(int mem_system_simulation, double mean){
    double val1 = randomNumberGeneratorUniform();
    double val2 = randomNumberGeneratorUniform();

    double sigma_val = mem_system_simulation/6.0;
    double rand_val_normal_dist = cos(2 * M_PI * val2)*sqrt(-2.0 * log(val1));

    int val = rand_val_normal_dist*sigma_val + mean;

    if(val < 0){
        val = 2*mean-val;
    }

    return val%mem_system_simulation;
    
}



void simulate(double *avg_wait, int avg_wait_l, int procs, char dist){


    int mem_system_simulation = 1, cycle_count = 1;
    int processors_access_denial_cnt_lst[procs];
    int processors_state_lst[procs];
    int *mean_proc;


    int i,processor_num, memory,first_access_denied_processor_identified, first_acc_denied;

    float prev_each_mem_simulation_system_avg_wait_time = 0;
    float current_each_mem_simulation_system_avg_wait_time = 0;
    float diff_btw_diff_simulations_avg_wait_time = 0;
    float processors_avg_access_time_lst[procs]; 

    /*
    mem_system_simulation goes from 1 to 2048, depends on number of memory modules
    */
    for (mem_system_simulation = 1; mem_system_simulation <= avg_wait_l; mem_system_simulation++){
        /*
            first_access_denied_processor_identified = 0/1
            0 - Not identified yet in current cycle
            1 - Already Identified in current cycle
        */
        first_access_denied_processor_identified = 0; i = 0;

        int mem_state_lst[mem_system_simulation];
        int diff_in_acc_time_with_in_limit_cnt = 0;

        /*
            For every memory system simulation we are updating following buffers - 

            #updating each element of list consisting of each processor access 
            denail count for each system memory simulation  to 0

            #updating each element of list consisting of each processor state 
            to -1 (which meant processor is ready to go and connect to a memory module available)
        */
        
        processor_num = 0;
        while (processor_num < procs) {
            processors_access_denial_cnt_lst[processor_num] = 0;
            processors_state_lst[processor_num] = -1;
            ++processor_num;
        }

        if(dist == 'n') {
                mean_proc = generateMeanProcs(procs, mem_system_simulation);
        }


        for(cycle_count = 1; cycle_count <=TOTAL_NO_OF_CYCLES; ++cycle_count){
            
            /* 
            Free Memory buffer before start of every cycle 
            */
            int mem_indx = 0;

            while (mem_indx < mem_system_simulation){
                mem_state_lst[mem_indx] = 0;
                ++mem_indx;
            }

            if (first_access_denied_processor_identified == 1){
                i = first_acc_denied;
                first_access_denied_processor_identified = 0;
            }

            /*
                In every cycle we are going through every processor
                and processing them as per their current condition
            */
            int processor_count = 0;
            while (processor_count < procs){

                
                /* 
                    Type of distribution
                    Normal - 'n'
                    Uniform - 'u'
                */
                if (dist == 'u'){
                    memory =  (rand()/(1.0 + RAND_MAX)) * mem_system_simulation;
                }
	            else if(dist == 'n'){
                    memory = randomNumberGeneratorNormal(mem_system_simulation,(double)mean_proc[i]);
                }

                /*
                    Processor ready to accept memory connection but needs to check if
                    memory is free or not
                */
                if (processors_state_lst[i] == -1){
                    
                    /*
                        Memory is free, So we mark memory as occupied now
                    */
                    if(mem_state_lst[memory] == 0){
                        mem_state_lst[memory] = 1;
                    }
                    /*
                        Memory is not free
                    */
                    else{
                        /*
                            Memory is not free, we check if Access denied first processor is identified
                            or not. If already identified ignore, else catch the current i and mark it
                            as first access denied processor. Next increment access denial cnt and mark
                            processor state at current index with memory indx it wanted to get connected to..
                        */
                        if (first_access_denied_processor_identified == 0){
                            first_acc_denied = i;
                            first_access_denied_processor_identified = 1; //identified
                        }
                        processors_access_denial_cnt_lst[i]++;
                        processors_state_lst[i] = memory;
                    }
                }
                /*
                    Processor not ready to accept memory connection but needs to check if
                    memory is free or not
                */
                else{
                    /*
                        Following if gets executed in new cycle and it starts with first access denied 
                        processor, if that memory is free which processor wants to access, allocate
                        it to processor and mark memory is not free
                    */
                    if(mem_state_lst[processors_state_lst[i]] == 0){
                        mem_state_lst[processors_state_lst[i]] = 1;
                        processors_state_lst[i] = -1;
                    }
                    /*
                        if memory which processor wants to access is not free -
                        then increment processor access denial counter, next see if it is
                        first access denail counter,if not ignore else catch it and tag it
                    */
                    else{
                        if(first_access_denied_processor_identified == 0){
                            first_acc_denied = i;
                            first_access_denied_processor_identified = 1;
                        }
                        processors_access_denial_cnt_lst[i]++;
                    }
                }
                ++processor_count;
                i = (i+1)%procs;

            }
            /*
                Condition to break out from clock cycles if access times difference does not vary 
                more than EPSILON for consecutively more than DIFF_IN_ACC_TIME_WITH_IN_LIMIT_VAL
            */

            processor_num = 0;
            int compute_avg_access_time=0;


            while(processor_num < procs){

                if((cycle_count - processors_access_denial_cnt_lst[processor_num]) == 0){
                    break;
                }

                if (processor_num == procs-1){
                    compute_avg_access_time = 1;
                }

                ++processor_num;
            }

            if (compute_avg_access_time == 1){

                /*
                    Processors access time's to memory computations/summation
                */
                processor_num = 0;
                float processors_access_times_summation = 0;

                while(processor_num < procs){

                    processors_avg_access_time_lst[processor_num] = 
                    cycle_count / (cycle_count - processors_access_denial_cnt_lst[processor_num]);
                    
                    processors_access_times_summation += processors_avg_access_time_lst[processor_num];
                    ++processor_num;
                }

                current_each_mem_simulation_system_avg_wait_time = 
                processors_access_times_summation / procs;

                diff_btw_diff_simulations_avg_wait_time = 
                fabs(prev_each_mem_simulation_system_avg_wait_time - current_each_mem_simulation_system_avg_wait_time);

                if(diff_btw_diff_simulations_avg_wait_time <= EPSILON){
                    diff_in_acc_time_with_in_limit_cnt += 1;
                }
                else{
                    diff_in_acc_time_with_in_limit_cnt = 0;
                }

                if((diff_in_acc_time_with_in_limit_cnt >= DIFF_IN_ACC_TIME_WITH_IN_LIMIT_VAL) && (diff_btw_diff_simulations_avg_wait_time <= EPSILON)){
                    break;
                }

                prev_each_mem_simulation_system_avg_wait_time = current_each_mem_simulation_system_avg_wait_time;
                processors_access_times_summation = 0;
            }
              
        }
        avg_wait[mem_system_simulation-1] = 
        current_each_mem_simulation_system_avg_wait_time;          
    }   
}