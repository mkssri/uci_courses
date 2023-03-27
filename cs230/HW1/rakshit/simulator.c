/* SUBMIT ONLY THIS FILE */
/* NAME: ....... */
/* UCI ID: .......*/

#include <stdio.h>
#include <stdlib.h>

void simulate(double *avg_wait, int avg_wait_l, int procs, char dist){
    /*simulation 1 to 2048, depends on memory modules*/
    int simulation = 1;
    int cycle = 1;
    float avg_acc[procs]; /**/
    int access_den[procs];
    int proc[procs];
    int i,j,counter, memory,flag, first_acc_denied;
    float average,sum = 0;
    
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
        for(cycle = 1; cycle <=1000000; ++cycle)
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
                memory = rand() % simulation;
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
        avg_acc[j] = 1000000 / (1000000 - access_den[j]);
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



/*


*/