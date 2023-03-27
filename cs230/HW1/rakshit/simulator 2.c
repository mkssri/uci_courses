/* SUBMIT ONLY THIS FILE */
/* NAME: ....... */
/* UCI ID: .......*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

double rand_gen() {
   /* return a uniformly distributed random value */
   return (rand()/(1.0 + RAND_MAX));
}

int* generate_mean_procs(int procs, int mem_size) {
    int *arr = (int *)malloc(sizeof(int) * procs);
    for(int i = 0; i < procs; i++) {
        arr[i] = (rand()/(1.0 + RAND_MAX)) * mem_size;
    }
    return arr;
}

int randN(double mean, double std) {
    double x1 = rand_gen();
    double x2 = rand_gen();

    double randN = cos(2 * M_PI * x2)*sqrt(-2.0 * log(x1));
    return (int) round(randN * std + mean);
}



void simulate(double *avg_wait, int avg_wait_l, int procs, char dist){

    srand(time(NULL));
    int simulation = 1;
    int cycle = 1;
    float avg_acc[procs];
    int access_den[procs];
    int proc[procs];
    int i,j,counter, memory,flag, first_acc_denied;
    float average,sum = 0;
    int *mean_proc;
    
    for (simulation = 1; simulation <= avg_wait_l; simulation++)
    {
        flag = 0; i = 0;
        int mem_free[simulation];
        for (j = 0; j < procs; ++j)
        {
            proc[j] = -1;
        }
        for (j = 0; j < procs; ++j)
        {
            access_den[j] = 0;
        }

	if(dist == 'n') 
	{
            mean_proc = generate_mean_procs(procs, simulation);
        }

        for(cycle = 1; cycle <=1000; ++cycle)
        {
	        for (j = 0; j < simulation; ++j)
        	{
            	mem_free[j] = 0;
       	    }
            if (flag == 1)
            {
                i = first_acc_denied;
                flag = 0;
            }
            counter = 0;
            while (counter < procs)
            {
                if (dist == 'u')
                {
                memory =  (rand()/(1.0 + RAND_MAX)) * simulation;
                }
	       else 
	       {
                memory = randN((double)mean_proc[i], simulation/6.0) % simulation;
                }
                
                
                
                
                
                
                
                if (proc[i] == -1)
                {
                    if(mem_free[memory] == 0)
                    {
                        mem_free[memory] = 1;
                    }
                    else
                    {
                        if (flag == 0)
                        {
                            first_acc_denied = i;
                            flag = 1;
                        }
                        access_den[i]++;
                        proc[i] = memory;
                    }
                }
                else
                {
                    if(mem_free[proc[i]] == 0)
                    {
                        mem_free[proc[i]] = 1;
                        proc[i] = -1;
                    }
                    else
                    {
                        if(flag == 0)
                        {
                            first_acc_denied = i;
                            flag = 1;
                        }
                        access_den[i]++;
                    }
                }
                ++counter;
                i = (i+1)%procs;
            }
        }         
    for (j = 0; j < procs; ++j)
    {
        avg_acc[j] = 1000 / (1000 - access_den[j]);
    }  
        
    for (j = 0; j < procs; ++j)
    {
        sum += avg_acc[j];
    }
          
    average = sum / procs;
    avg_wait[simulation-1] = average;
    sum = 0;
    }
    
}
