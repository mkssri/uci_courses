

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>


#define processor_count (10)
#define EPSILON (2)
int processor_load[processor_count];
int time_vector[processor_count];
int stop_flag = 0;
int total_initial_load;



int * adjust_processor_load(int l1, int l2, int l3){

    int avg;
    avg = floor((l1 + l2 + l3))/3;
    int local_excess = l2 - avg;
    static  int new_load[3];
    int diff;
    

    if(local_excess >0){
        l2 -= local_excess;
        if(l1 <= avg){
            diff = avg - l1;
            if(diff < local_excess){
                l1 += diff;
                local_excess -= diff;

            }else{
                l1 += local_excess;
                local_excess = 0;
            }
        }

        if(l3 <= avg){
            l3 += local_excess;
            local_excess = 0;
        }

        if(local_excess !=0){
            l2 += local_excess;
        }

    }
    new_load[0] = l1;
    new_load[1] = l2;
    new_load[2] = l3;

    return new_load;

}





int random_number_generator(int a, int b){
    int n;
    int min = a;
    int max = b;


    n = (rand()%( max - min +1)) + min;

//    printf("%d \n", n);
    return n;

}


int balance_check_flag(){
    

    int initial_avg_load = total_initial_load /processor_count;
    // int delta_allowed = (total_initial_load % processor_count);
    int delta_allowed = EPSILON*processor_count;

    int actual_deviation = 0;

    int i, diff;

    for (i = 0; i<processor_count; i++){
        diff = abs(processor_load[i] - initial_avg_load);
        actual_deviation += abs(processor_load[i] - initial_avg_load);

        // if(actual_deviation > delta_allowed || diff > EPSILON){
        //     stop_flag = 0;
        //     return stop_flag;
        // }

        if(diff > EPSILON){
            stop_flag = 0;
            return stop_flag;
        }

    }
    stop_flag = 1;
    // printf("%d \n", actual_deviation);
    // printf("%d \n", initial_avg_load);
    return stop_flag;



}


int index_min_time(int input_array[], int size){
    int i;
    int min_index = 0;
    int min = input_array[0];

    for (i = 1; i < size; i++){
        if(input_array[i] < min){
            min = input_array[i];
            min_index = i;
        }

    }

    return min_index;

}

int main(){

    int cycle = 0;
    total_initial_load = 0;
    int average = 0;

    srand(time(NULL));
    

    int i;
    for (i=0; i<processor_count; i++){
        processor_load[i] = random_number_generator(10,1000);
    //    printf("The initial load is %d , ", i, processor_load[i]);
    }
    printf("The initial load is: \n");
    for (i=0; i<processor_count; i++){
        printf("%d ,",processor_load[i]);
    }
    printf("\n");

    for (i=0; i<processor_count; i++){
        average += processor_load[i];
    }
    average = average / processor_count;

    printf("The Initial average load is: %d \n", average );

    for (i=0; i<processor_count; i++){
        total_initial_load +=  processor_load[i];
    }




    for (i=0; i<processor_count; i++){
        time_vector[i] = random_number_generator(100, 1000);
//        printf("for process %d time is %d \n", i, time_vector[i]);

    }


    while (!(stop_flag)) {
        int processor_index;
        int *temp_array;
        int left_neighbor;
        int right_neightbor;
        int local_processor;

        processor_index = index_min_time(time_vector, processor_count);
        local_processor = processor_index;
        if (local_processor == 0) {
            left_neighbor = processor_count - 1;
            right_neightbor = local_processor + 1;
        } else if (local_processor == processor_count - 1) {
            left_neighbor = local_processor - 1;
            right_neightbor = 0;
        } else {
            left_neighbor = local_processor - 1;
            right_neightbor = local_processor + 1;
        }

        temp_array = adjust_processor_load(processor_load[left_neighbor], processor_load[local_processor],processor_load[right_neightbor]);
        processor_load[left_neighbor] = *temp_array;
        processor_load[local_processor] = *(temp_array +1);
        processor_load[right_neightbor] = *(temp_array+2);

        time_vector[local_processor] = random_number_generator(100, 1000);
        stop_flag = balance_check_flag();
        cycle += 1;
        // printf("cyles: %d\n", cycle);
        



    }
    // printf("Completed \n");
    printf("\n cyles: %d\n", cycle);

    printf("The Final load is: \n");
    for (i=0; i<processor_count; i++){
        printf("%d ,",processor_load[i]);
    }




    return cycle;

}




