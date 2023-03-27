/* SUBMIT ONLY THIS FILE */
/* NAME: ....... */
/* UCI ID: .......*/

// only include standard libraries.
#include <stdio.h>

void simulate(double *avg_wait, int avg_wait_l, int procs, char dist){
   
    /*simulation 1 to 2048, depends on memory modules*/
    int mem_system_simulation = 1, cycle_count = 1;
    int processors_access_denial_lst[procs];
    int proc[procs];

    float processors_avg_access_time_lst[procs]; 
    int i,j,counter, memory,flag, first_acc_denied;
    float average,sum = 0;
    
    for (mem_system_simulation = 1; mem_system_simulation <= avg_wait_l; mem_system_simulation++)
    {
        flag = 0; i = 0;
        int mem_free[mem_system_simulation];
        for (j = 0; j < procs; ++j)
        {
            proc[j] = -1;
        }
        for (j = 0; j < procs; ++j)
        {
            processors_access_denial_lst[j] = 0;
        }
        for(cycle_count = 1; cycle_count <=1000000; ++cycle_count)
        {
	        for (j = 0; j < mem_system_simulation; ++j)
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
                memory = rand() % mem_system_simulation;
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
                        processors_access_denial_lst[i]++;
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
                        processors_access_denial_lst[i]++;
                    }
                }
                ++counter;
                i = (i+1)%procs;
            }
        }         
    for (j = 0; j < procs; ++j)
    {
        processors_avg_access_time_lst[j] = 1000000 / (1000000 - processors_access_denial_lst[j]);
    }  
        
    for (j = 0; j < procs; ++j)
    {
        sum += processors_avg_access_time_lst[j];
    }
          
    average = sum / procs;

    avg_wait[mem_system_simulation-1] = average;
    sum = 0;
    }
    
}