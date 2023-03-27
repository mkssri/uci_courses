/* SUBMIT ONLY THIS FILE */
/* NAME: ....... */
/* UCI ID: .......*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

double rand_gen() {
   /* return a uniformly distributed random value */
   return ( (double)(rand()) + 1. )/( (double)(RAND_MAX) + 1. );
}

int* generate_mean_procs(int procs, int mem_size) {
    int *arr = (int *)malloc(sizeof(int) * procs);
    for(int i = 0; i < procs; i++) {
        arr[i] = rand() % mem_size;
    }
    return arr;
}

int randN(double mean, double std) {
    double x1 = rand_gen();
    double x2 = rand_gen();

    double y = sin(2 * M_PI * x2)*sqrt(-2.0 * log(x1));
    return (int) round(y * std + mean);
}



void simulate(double *avg_wait, int avg_wait_l, int procs, char dist){

    srand(time(NULL));
    int simulation;
    int cycle;
    double avg_acc[procs];
    int access_den[procs];
    int max_acc_den;
    int proc[procs];
    int visited[procs];
    int i,j,counter[procs], memory, first_acc_denied, flag;
    double average,sum;
    int *mean_proc;
    
    for (simulation = 1; simulation <= avg_wait_l; simulation++)
    {
        max_acc_den = 0,flag =0;
        int mem_free[simulation];
        double wbar = 0.0, wprev = 0.0, wcur = 0.0;
        for (j = 0; j < procs; ++j)
        {
            proc[j] = 0;
        }
        for (j = 0; j < procs; ++j)
        {
            access_den[j] = 0;
        }
        for (j = 0; j < procs; ++j)
        {
            counter[j] = 0;
        }
	    if(dist == 'n') 
    	{
            mean_proc = generate_mean_procs(procs, simulation);
        }

        for(cycle = 1; cycle <=1000000; ++cycle)
        {
	        for (j = 0; j < simulation; ++j)
        	{
            	mem_free[j] = 0;
       	    }
       	    
       	    for(j = 0; j < procs; ++j)
       	    {
       	        visited[j] = 0;
       	    }
       	    
       	    for (i = 0; i< procs; ++i)
       	    {
       	        
       	        if (dist == 'u')
                {
                        memory = rand() % simulation;
                    }
	                else 
	                {
                        memory = randN((double)mean_proc[i], simulation/6) % simulation;
                    }
                    
                    if(access_den[i] == 0)
                    {
                        proc[i] = memory;
                    }
       	        
       	    }
       	    
            int temp = max_acc_den;
            while (temp >= 0)
            {
                i = 0;
                while(i < procs)
                {
                    if(access_den[i] == temp && visited[i] == 0)
                    {
       	            
                        if (mem_free[proc[i]] == 0)
                        {
                            mem_free[proc[i]]  = 1;
                            access_den[i] = 0;
                            visited[i] = 1;
                            counter[i]++;
                        }
                        else
                        {
                            access_den[i]++;
                            if(max_acc_den > access_den[i])
                            {
                                max_acc_den = access_den[i];
                            }
                            visited[i] = 1;
                        }
                    }
                    
                i++;
                }
             temp--;
            }

	        if(flag == 0) {
                i = 0;
                while(i<procs) {
                    if(counter[i] == 0)
                        break;
                    i++;
                }
            }
        
            if(i == procs)
                flag = 1;
            
            if(flag == 1) {
        
                sum = 0.0;
                for(i=0; i<procs; i++) {
                    sum += ((double)cycle/(double)counter[i]);
                }
        
                wbar = sum;
        
                wprev = wcur;
                wcur = (double)wbar/(double)procs;
        
                int check = 1-(wprev/wcur);
                if(wprev != 0 && abs(check) < 0.002)
                    break;
	       }
        }         
    avg_wait[simulation-1] = wcur;

    }
    
}