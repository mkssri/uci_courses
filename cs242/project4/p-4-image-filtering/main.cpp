#include <stdio.h>
#include <stdlib.h>
#include <string.h>   
#include <time.h>
#include <dirent.h>

#define BUF_SIZE 1000  

#include "conv.h"

int main(int argc, char** argv) {
	struct timespec start, end;
    double time_taken;
	FILE* fp;
	unsigned int width, height;

    struct dirent *ptr_de;  
    DIR *ptr_dr = opendir("images");
    
	if (ptr_dr == NULL)  
    {
        printf("Unable to open current directory" );
        return 0;
    }

	printf("+----------------+--------------------------------------+--------------------------------------+\n");
	printf("| Image          | Out of Place                         | In of Place                          |\n");
	printf("+----+-----------+------------+-------------+-----------+------------+-------------+-----------+\n");
	printf("| ID | Size      | Sequential | MT by Color | MT by Row | Sequential | MT by Color | MT by Row |\n");
	printf("+----+-----------+------------+-------------+-----------+------------+-------------+-----------+\n");
	int i = 0;
    while ((ptr_de = readdir(ptr_dr)) != NULL){

		char file[128] = "images/";

	
        if(ptr_de->d_type==DT_REG){
			strcat(file, ptr_de->d_name);
			//printf("working on image: %s\n", file);
			fp = fopen(file, "rb");
			char buffer[BUF_SIZE];
			char* tempBuffer;
			fgets(buffer, sizeof(buffer), fp);
			if (strcmp(buffer, "P6\n") != 0) {
				printf("wrong file type, use .ppm \n");
				return 0;
			}
			fgets(buffer, sizeof(buffer), fp);
			while (buffer[0] == '#') {
				fgets(buffer, sizeof(buffer), fp);
			}
			width = (int)strtol(buffer, &tempBuffer, 10);
			height = (int)strtol(tempBuffer, &tempBuffer, 10);
			printf("| %2d | %4dx%4d |", i, width, height);
			i++;
			fgets(buffer, sizeof(buffer), fp);
			if (strcmp(buffer, "255\n") != 0) {
				printf("wrong picture value \n");
			}

			Mat orig_im[3];
			mat_init(&orig_im[0], width, height);
			mat_init(&orig_im[1], width, height);
			mat_init(&orig_im[2], width, height);
			storeImage(&orig_im[0], &orig_im[1], &orig_im[2], fp);

			//create filter
			Mat filter;
			mat_init(&filter, 3,3);
			filterAssign(&filter, 0);

			//conv 3x3 out place
			Mat new_image_A[3];
			mat_init(&new_image_A[0], width, height);
			mat_init(&new_image_A[1], width, height);
			mat_init(&new_image_A[2], width, height);
			clock_gettime(CLOCK_MONOTONIC, &start);
			conv_3x3_out(&orig_im[0], &orig_im[1], &orig_im[2], &new_image_A[0], &new_image_A[1], &new_image_A[2], &filter);
			clock_gettime(CLOCK_MONOTONIC, &end);
			time_taken = (end.tv_sec - start.tv_sec) * 1e9;
			time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
			//printf("conv time taken for out of place: %f\n", time_taken);
			printf("   %7f |", time_taken);


			//conv 3x3 out place multi threaded on data using omp
			Mat new_image_B[3];
			mat_init(&new_image_B[0], width, height);
			mat_init(&new_image_B[1], width, height);
			mat_init(&new_image_B[2], width, height);
			clock_gettime(CLOCK_MONOTONIC, &start);
			conv_3x3_out_muli_color(&orig_im[0], &orig_im[1], &orig_im[2], &new_image_B[0], &new_image_B[1], &new_image_B[2], &filter);
			clock_gettime(CLOCK_MONOTONIC, &end);
			time_taken = (end.tv_sec - start.tv_sec) * 1e9;
			time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
			//printf("conv time taken for out of place multi-threaded by rows: %f\n", time_taken);
			printf("    %7f |", time_taken);


			//conv 3x3 out place multi threaded on colors using pthreads
			Mat new_image_C[3];
			mat_init(&new_image_C[0], width, height);
			mat_init(&new_image_C[1], width, height);
			mat_init(&new_image_C[2], width, height);
			clock_gettime(CLOCK_MONOTONIC, &start);
			conv_3x3_out_muli_data(&orig_im[0], &orig_im[1], &orig_im[2], &new_image_C[0], &new_image_C[1], &new_image_C[2], &filter);
			clock_gettime(CLOCK_MONOTONIC, &end);
			time_taken = (end.tv_sec - start.tv_sec) * 1e9;
			time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
			//printf("conv time taken for out of place multi-threaded by color: %f\n", time_taken);
			printf("  %7f |", time_taken);


			//conv 3x3 in place
			Mat image_3x3_in[3];
			mat_init(&image_3x3_in[0], width, height);
			mat_init(&image_3x3_in[1], width, height);
			mat_init(&image_3x3_in[2], width, height);
			mat_copy(&orig_im[0],&image_3x3_in[0]);
			mat_copy(&orig_im[1],&image_3x3_in[1]);
			mat_copy(&orig_im[2],&image_3x3_in[2]);
			clock_gettime(CLOCK_MONOTONIC, &start);
			conv_3x3(&image_3x3_in[0], &image_3x3_in[1], &image_3x3_in[2], &filter);
			clock_gettime(CLOCK_MONOTONIC, &end);
			time_taken = (end.tv_sec - start.tv_sec) * 1e9;
			time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
			//printf("conv time taken for in place: %f\n", time_taken);
			printf("   %7f |", time_taken);


			//conv 3x3 in place multi threaded
			Mat image_3x3_in_A[3];
			mat_init(&image_3x3_in_A[0], width, height);
			mat_init(&image_3x3_in_A[1], width, height);
			mat_init(&image_3x3_in_A[2], width, height);
			mat_copy(&orig_im[0],&image_3x3_in_A[0]);
			mat_copy(&orig_im[1],&image_3x3_in_A[1]);
			mat_copy(&orig_im[2],&image_3x3_in_A[2]);
			clock_gettime(CLOCK_MONOTONIC, &start);
			conv_3x3_in_color_multi(&image_3x3_in_A[0], &image_3x3_in_A[1], &image_3x3_in_A[2], &filter);
			clock_gettime(CLOCK_MONOTONIC, &end);
			time_taken = (end.tv_sec - start.tv_sec) * 1e9;
			time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
			//printf("conv time taken for in place MT by colors: %f\n", time_taken);
			printf("    %7f |", time_taken);
			//mat_equal(&image_3x3_in[1], &image_3x3_in_A[1]);
			//writeImage(&image_3x3_in_C[0], &image_3x3_in_C[1], &image_3x3_in_C[2]);

			Mat image_3x3_in_B[3];
			mat_init(&image_3x3_in_B[0], width, height);
			mat_init(&image_3x3_in_B[1], width, height);
			mat_init(&image_3x3_in_B[2], width, height);
			mat_copy(&orig_im[0],&image_3x3_in_B[0]);
			mat_copy(&orig_im[1],&image_3x3_in_B[1]);
			mat_copy(&orig_im[2],&image_3x3_in_B[2]);
			clock_gettime(CLOCK_MONOTONIC, &start);
			conv_3x3_in_data_multi(&image_3x3_in_B[0], &image_3x3_in_B[1], &image_3x3_in_B[2], &filter);
			clock_gettime(CLOCK_MONOTONIC, &end);
			time_taken = (end.tv_sec - start.tv_sec) * 1e9;
			time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
			//printf("conv time taken for in place MT by colors: %f\n", time_taken);
			printf("  %f |\n", time_taken);
		}
		
					
	}

	printf("+----+-----------+------------+-------------+-----------+------------+-------------+-----------+\n");
	
	fclose(fp);
    closedir(ptr_dr);    

	return 0;
}
