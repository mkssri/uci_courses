#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <pthread.h>

#include "conv.h"

#define omp_thread_count 4



void mat_init(Mat* mat, unsigned int m, unsigned int n) {
    if (m == 0 || n == 0) {
        printf("mat_init: Matrix size invalid.\n");
        exit(1);
    }
    mat->ptr = (double*)calloc(m * n, sizeof(double));
    if (mat->ptr == NULL) {
        printf("mat_init: Couldn't allocate memory for matrix.\n");
        exit(1);
    }
    mat->m = m;
    mat->n = n;
    return;
}

void mat_copy(Mat* from, Mat* to) {
    for (int i = 0; i < from->m * from->n; i++) {
        to->ptr[i] = from->ptr[i];
    }
}

void filterAssign(Mat* mat, int option) {
    if (mat->m == 2) {
        mat->ptr[0] = 0.25;
        mat->ptr[1] = 0.25;
        mat->ptr[2] = 0.25;
        mat->ptr[3] = 0.25;
    }
    else {
        if (option == 0) {
            mat->ptr[0] = 0.0;
            mat->ptr[1] = -1.0;
            mat->ptr[2] = 0.0;

            mat->ptr[3] = -1.0;
            mat->ptr[4] = 4.0;
            mat->ptr[5] = -1.0;

            mat->ptr[6] = 0.0;
            mat->ptr[7] = -1.0;
            mat->ptr[8] = 0.0;
        }
        else if(option == 1) {
            mat->ptr[0] = -1.0;
            mat->ptr[1] = -1.0;
            mat->ptr[2] = -1.0;

            mat->ptr[3] = -1.0;
            mat->ptr[4] = 8.0;
            mat->ptr[5] = -1.0;

            mat->ptr[6] = -1.0;
            mat->ptr[7] = -1.0;
            mat->ptr[8] = -1.0;
        }
        else {
            mat->ptr[0] = 0;
            mat->ptr[1] = -0.5;
            mat->ptr[2] = 0;

            mat->ptr[3] = -0.5;
            mat->ptr[4] = 3;
            mat->ptr[5] = -0.5;

            mat->ptr[6] = 0;
            mat->ptr[7] = -0.5;
            mat->ptr[8] = 0;
        }

    }
}

void mat_print(Mat* mat, int flag) {
    unsigned int i, j, m = mat->m, n = mat->n;
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            printf("  %f", mat->ptr[i * n + j]);
        }
        printf("\n");
    }
    return;
}

void storeImage(Mat* to, Mat* green, Mat* blue, FILE* fp) {
    unsigned char buffer[3];
    int i = 0;
    while (fread(&buffer, sizeof(unsigned char), 3, fp)) {
        to->ptr[i] = (double)buffer[0];
        green->ptr[i] = (double)buffer[1];
        blue->ptr[i] = (double)buffer[2];
        i++;
    }
    return;
}

void writeImage(Mat* to, Mat* green, Mat* blue) {
    int n = to->n;
    int m = to->m;
    FILE* fp;
    fp = fopen("result/temp.ppm", "wb");
    if (fp == NULL) {
        perror("Can't open file");
    }
    fprintf(fp, "P6\n%d %d\n%d\n", m, n, 255);
    for (int i = 0; i < m*n; i++) {
        int tempto = (int)to->ptr[i];
        int tempGreen = (int)green->ptr[i];
        int tempBlue = (int)blue->ptr[i];
        fprintf(fp, "%c%c%c", (char)tempto, (char)tempGreen, (char)tempBlue);
    }
    fclose(fp);

}

void swapArray(double** A, double** B) {
    double* temp = *A;
    *A = *B;
    *B = temp;
}

void conv_2x2(Mat* red, Mat* green, Mat* blue, Mat* filter) {
    int m = red->m;
    int n = red->n;
    int i = 0;
    for (int row = 0; row < n -1; row+=2) {
        for (int column = 0; column < m -1; column+=2) {
            double sumR = 0;
            double sumG = 0;
            double sumB = 0;

            sumR += red->ptr[row * m + column] *  filter->ptr[0];
            sumR += red->ptr[row * m + column+1] * filter->ptr[1];
            sumR += red->ptr[(row +1)* m + column] * filter->ptr[2];
            sumR += red->ptr[(row + 1) * m + column+1] * filter->ptr[3];
            if (sumR < 0) {
                sumR = 0;
            }
            if (sumR > 255) {
                sumR = 255;
            }
            red->ptr[i] = sumR;
            
            sumG += green->ptr[row * m + column] * filter->ptr[0];
            sumG += green->ptr[row * m + column + 1] * filter->ptr[1];
            sumG += green->ptr[(row + 1) * m + column] * filter->ptr[2];
            sumG += green->ptr[(row + 1) * m + column + 1] * filter->ptr[3];
            if (sumG < 0) {
                sumG = 0;
            }
            if (sumG > 255) {
                sumG = 255;
            }
            green->ptr[i] = sumG;

            sumB += blue->ptr[row * m + column] * filter->ptr[0];
            sumB += blue->ptr[row * m + column + 1] * filter->ptr[1];
            sumB += blue->ptr[(row + 1) * m + column] * filter->ptr[2];
            sumB += blue->ptr[(row + 1) * m + column + 1] * filter->ptr[3];
            if (sumB < 0) {
                sumB = 0;
            }
            if (sumB > 255) {
                sumB = 255;
            }
            blue->ptr[i] = sumB;
            i++;
        }
    }
    red->m = m/2;
    red->n = n/2;
    green->m = m / 2;
    green->n = n / 2;
    blue->m = m / 2;
    blue->n = n / 2;
}

//n = 665, m =1000
void conv_3x3(Mat* red, Mat* green, Mat* blue, Mat* filter) {
    int m = red->m;
    int n = red->n;
    double *lastR = (double*) calloc(m, sizeof(double));
    double *currentR = (double*)calloc(m, sizeof(double));

    double* lastG = (double*)calloc(m, sizeof(double));
    double* currentG = (double*)calloc(m, sizeof(double));

    double* lastB = (double*)calloc(m, sizeof(double));
    double* currentB = (double*)calloc(m, sizeof(double));


    double* plastR = lastR;
    double* pcurrentR = currentR;

    double* plastG = lastG;
    double* pcurrentG = currentG;

    double* plastB = lastB;
    double* pcurrentB = currentB;

    for (int row = 0; row < n; row++) {
        for (int column = 0; column < m; column++) {
            double sumR = 0;
            double sumG = 0;
            double sumB = 0;

          
            //index corner 1
            if (column != 0 && row != 0) {
                sumR += filter->ptr[0] * plastR[column - 1];
                sumG += filter->ptr[0] * plastG[column - 1];
                sumB += filter->ptr[0] * plastB[column - 1];
            }

            if (row != 0) {
                sumR += filter->ptr[1] * plastR[column];
                sumG += filter->ptr[1] * plastG[column];
                sumB += filter->ptr[1] * plastB[column];
            }
            
            
            if (column != m-1  && row != 0) {
                sumR += filter->ptr[2] * plastR[column + 1];
                sumG += filter->ptr[2] * plastG[column + 1];
                sumB += filter->ptr[2] * plastB[column + 1];
            }
           
            if (column != 0) {
                sumR += filter->ptr[3] * pcurrentR[column - 1];
                sumG += filter->ptr[3] * pcurrentG[column - 1];
                sumB += filter->ptr[3] * pcurrentB[column - 1];
            }

            
           if (column != m - 1) {
                sumR += filter->ptr[5] * red->ptr[row *m+ (column + 1)];
                sumG += filter->ptr[5] * green->ptr[row *m+ (column + 1)];
                sumB += filter->ptr[5] * blue->ptr[row *m+ (column + 1)];
            }

            if (row != n - 1 && column != 0) {
                sumR += filter->ptr[6] * red->ptr[(row+ 1) *m+ column - 1];
                sumG += filter->ptr[6] * green->ptr[(row+ 1) *m+ column - 1];
                sumB += filter->ptr[6] * blue->ptr[(row+ 1) *m+ column - 1];
            }
            
            
            if (row != n - 1) {
                sumR += filter->ptr[7] * red->ptr[(row + 1) * m + column];
                sumG += filter->ptr[7] * green->ptr[(row + 1) * m + column];
                sumB += filter->ptr[7] * blue->ptr[(row + 1) * m + column];
            }
            
            if (row != n - 1 && column != m - 1) {
                sumR += filter->ptr[8] * red->ptr[(row + 1) * m + column + 1];
                sumG += filter->ptr[8] * green->ptr[(row + 1) * m + column + 1];
                sumB += filter->ptr[8] * blue->ptr[(row + 1) * m + column + 1];
            }
            
            pcurrentR[column] = red->ptr[row * m + column];
            red->ptr[row* m + column] *= filter->ptr[4];
            red->ptr[row* m + column] += sumR;
            if (red->ptr[row * m + column] < 0) {
                red->ptr[row * m + column] = 0;
            }
            if (red->ptr[row * m + column] > 255) {
                red->ptr[row * m + column] = 255;
            }
            pcurrentG[column] = green->ptr[row* m + column];
            green->ptr[row* m + column] *= filter->ptr[4];
            green->ptr[row* m + column] += sumG;
            if (green->ptr[row * m + column] < 0) {
                green->ptr[row * m + column] = 0;
            }
            if (green->ptr[row * m + column] > 255) {
                green->ptr[row * m + column] = 255;
            }

            pcurrentB[column] = blue->ptr[row* m + column];
            blue->ptr[row* m + column] *= filter->ptr[4];
            blue->ptr[row* m + column] += sumB;
            if (blue->ptr[row * m + column] < 0) {
                blue->ptr[row * m + column] = 0;
            }
            if (blue->ptr[row * m + column] > 255) {
                blue->ptr[row * m + column] = 255;
            }
        }
        swapArray(&plastR, &pcurrentR);
        swapArray(&plastG, &pcurrentG);
        swapArray(&plastB, &pcurrentB);
    }
}

void conv_3x3_out (Mat* red, Mat* green, Mat* blue, Mat* red_o, Mat* green_o, Mat* blue_o, Mat* filter) {
    int m = red->m;
    int n = red->n;
    for (int row = 0; row < n; row++) {
        for (int column = 0; column < m; column++) {
            double sumR = 0;
            double sumG = 0;
            double sumB = 0;
            //index corner 1
            if (column != 0 && row != 0) {
                sumR += filter->ptr[0] * red->ptr[(row-1) * m + column -1];
                sumG += filter->ptr[0] * green->ptr[(row - 1) * m + column - 1];
                sumB += filter->ptr[0] * blue->ptr[(row - 1) * m + column - 1];
            }

            if (row != 0) {
                sumR += filter->ptr[1] * red->ptr[(row - 1) * m + column];
                sumG += filter->ptr[1] * green->ptr[(row - 1) * m + column];
                sumB += filter->ptr[1] * blue->ptr[(row - 1) * m + column ];
            }


            if (column != m - 1 && row != 0) {
                sumR += filter->ptr[2] * red->ptr[(row - 1) * m + column + 1];
                sumG += filter->ptr[2] * green->ptr[(row - 1) * m + column + 1];
                sumB += filter->ptr[2] * blue->ptr[(row - 1) * m + column + 1];
            }

            if (column != 0) {
                sumR += filter->ptr[3] * red->ptr[row* m + column - 1];
                sumG += filter->ptr[3] * green->ptr[row * m + column - 1];
                sumB += filter->ptr[3] * blue->ptr[row * m + column - 1];
            }


            if (column != m - 1) {
                sumR += filter->ptr[5] * red->ptr[row * m + (column + 1)];
                sumG += filter->ptr[5] * green->ptr[row * m + (column + 1)];
                sumB += filter->ptr[5] * blue->ptr[row * m + (column + 1)];
            }

            if (row != n - 1 && column != 0) {
                sumR += filter->ptr[6] * red->ptr[(row + 1) * m + column - 1];
                sumG += filter->ptr[6] * green->ptr[(row + 1) * m + column - 1];
                sumB += filter->ptr[6] * blue->ptr[(row + 1) * m + column - 1];
            }


            if (row != n - 1) {
                sumR += filter->ptr[7] * red->ptr[(row + 1) * m + column];
                sumG += filter->ptr[7] * green->ptr[(row + 1) * m + column];
                sumB += filter->ptr[7] * blue->ptr[(row + 1) * m + column];
            }
            if (row != n - 1 && column != m - 1) {
                sumR += filter->ptr[8] * red->ptr[(row + 1) * m + column + 1];
                sumG += filter->ptr[8] * green->ptr[(row + 1) * m + column + 1];
                sumB += filter->ptr[8] * blue->ptr[(row + 1) * m + column + 1];
            }
              
            red_o->ptr[row * m + column] = red->ptr[row * m + column] * filter->ptr[4];
            red_o->ptr[row * m + column] += sumR;

            if (red_o->ptr[row * m + column] < 0) {
                red_o->ptr[row * m + column] = 0;
            }
            if (red_o->ptr[row * m + column] > 255) {
                red_o->ptr[row * m + column] = 255;
            }

            green_o->ptr[row * m + column] = green->ptr[row * m + column] * filter->ptr[4];
            green_o->ptr[row * m + column] += sumG;

            if (green_o->ptr[row * m + column] < 0) {
                green_o->ptr[row * m + column] = 0;
            }
            if (green_o->ptr[row * m + column] > 255) {
                green_o->ptr[row * m + column] = 255;
            }

            blue_o->ptr[row * m + column] = blue->ptr[row * m + column] * filter->ptr[4];
            blue_o->ptr[row * m + column] += sumB;
            if (blue_o->ptr[row * m + column] < 0) {
                blue_o->ptr[row * m + column] = 0;
            }
            if (blue_o->ptr[row * m + column] > 255) {
                blue_o->ptr[row * m + column] = 255;
            }
          }
     }
}

//mulithreaded on the data
void conv_3x3_out_muli_data (Mat* red, Mat* green, Mat* blue, Mat* red_o, Mat* green_o, Mat* blue_o, Mat* filter) {
    int m = red->m;
    int n = red->n;

    #pragma omp parallel for
    for (int row = 0; row < n; row++) {
        for (int column = 0; column < m; column++) {
            double sumR = 0;
            double sumG = 0;
            double sumB = 0;
            //index corner 1
            if (column != 0 && row != 0) {
                sumR += filter->ptr[0] * red->ptr[(row-1) * m + column -1];
                sumG += filter->ptr[0] * green->ptr[(row - 1) * m + column - 1];
                sumB += filter->ptr[0] * blue->ptr[(row - 1) * m + column - 1];
            }

            if (row != 0) {
                sumR += filter->ptr[1] * red->ptr[(row - 1) * m + column];
                sumG += filter->ptr[1] * green->ptr[(row - 1) * m + column];
                sumB += filter->ptr[1] * blue->ptr[(row - 1) * m + column ];
            }


            if (column != m - 1 && row != 0) {
                sumR += filter->ptr[2] * red->ptr[(row - 1) * m + column + 1];
                sumG += filter->ptr[2] * green->ptr[(row - 1) * m + column + 1];
                sumB += filter->ptr[2] * blue->ptr[(row - 1) * m + column + 1];
            }

            if (column != 0) {
                sumR += filter->ptr[3] * red->ptr[row* m + column - 1];
                sumG += filter->ptr[3] * green->ptr[row * m + column - 1];
                sumB += filter->ptr[3] * blue->ptr[row * m + column - 1];
            }


            if (column != m - 1) {
                sumR += filter->ptr[5] * red->ptr[row * m + (column + 1)];
                sumG += filter->ptr[5] * green->ptr[row * m + (column + 1)];
                sumB += filter->ptr[5] * blue->ptr[row * m + (column + 1)];
            }

            if (row != n - 1 && column != 0) {
                sumR += filter->ptr[6] * red->ptr[(row + 1) * m + column - 1];
                sumG += filter->ptr[6] * green->ptr[(row + 1) * m + column - 1];
                sumB += filter->ptr[6] * blue->ptr[(row + 1) * m + column - 1];
            }


            if (row != n - 1) {
                sumR += filter->ptr[7] * red->ptr[(row + 1) * m + column];
                sumG += filter->ptr[7] * green->ptr[(row + 1) * m + column];
                sumB += filter->ptr[7] * blue->ptr[(row + 1) * m + column];
            }
            if (row != n - 1 && column != m - 1) {
                sumR += filter->ptr[8] * red->ptr[(row + 1) * m + column + 1];
                sumG += filter->ptr[8] * green->ptr[(row + 1) * m + column + 1];
                sumB += filter->ptr[8] * blue->ptr[(row + 1) * m + column + 1];
            }
              
            red_o->ptr[row * m + column] = red->ptr[row * m + column] * filter->ptr[4];
            red_o->ptr[row * m + column] += sumR;

            if (red_o->ptr[row * m + column] < 0) {
                red_o->ptr[row * m + column] = 0;
            }
            if (red_o->ptr[row * m + column] > 255) {
                red_o->ptr[row * m + column] = 255;
            }

            green_o->ptr[row * m + column] = green->ptr[row * m + column] * filter->ptr[4];
            green_o->ptr[row * m + column] += sumG;

            if (green_o->ptr[row * m + column] < 0) {
                green_o->ptr[row * m + column] = 0;
            }
            if (green_o->ptr[row * m + column] > 255) {
                green_o->ptr[row * m + column] = 255;
            }

            blue_o->ptr[row * m + column] = blue->ptr[row * m + column] * filter->ptr[4];
            blue_o->ptr[row * m + column] += sumB;
            if (blue_o->ptr[row * m + column] < 0) {
                blue_o->ptr[row * m + column] = 0;
            }
            if (blue_o->ptr[row * m + column] > 255) {
                blue_o->ptr[row * m + column] = 255;
            }
          }
     }

}
typedef struct thread_data{
    Mat* to;
    Mat* from;
    Mat* filter;
} Matrixs;

void* helper(void* arg){
    Matrixs *per = (Matrixs*) arg;
    Mat* from = per->from;
    Mat* to = per->to;
    Mat* filter = per->filter;
    int m = to->m;
    int n = to->n;
    for (int row = 0; row < n; row++) {
        for (int column = 0; column < m; column++) {
            double sum = 0;

            //index corner 1
            if (column != 0 && row != 0) {
                sum += filter->ptr[0] * to->ptr[(row-1) * m + column -1];
                
            }

            if (row != 0) {
                sum += filter->ptr[1] * to->ptr[(row - 1) * m + column];
            }


            if (column != m - 1 && row != 0) {
                sum += filter->ptr[2] * to->ptr[(row - 1) * m + column + 1];
            }

            if (column != 0) {
                sum += filter->ptr[3] * to->ptr[row* m + column - 1];
            }


            if (column != m - 1) {
                sum += filter->ptr[5] * to->ptr[row * m + (column + 1)];

            }

            if (row != n - 1 && column != 0) {
                sum += filter->ptr[6] * to->ptr[(row + 1) * m + column - 1];
            }


            if (row != n - 1) {
                sum += filter->ptr[7] * to->ptr[(row + 1) * m + column];
            }

            if (row != n - 1 && column != m - 1) {
                sum += filter->ptr[8] * to->ptr[(row + 1) * m + column + 1];
            }
              
            from->ptr[row * m + column] = to->ptr[row * m + column] * filter->ptr[4];
            from->ptr[row * m + column] += sum;

            if (from->ptr[row * m + column] < 0) {
                from->ptr[row * m + column] = 0;
            }
            if (from->ptr[row * m + column] > 255) {
                from->ptr[row * m + column] = 255;
            }
          }
     }
     pthread_exit(NULL);
}


//mulithreaded based on color
void conv_3x3_out_muli_color(Mat* red, Mat* green, Mat* blue, Mat* red_o, Mat* green_o, Mat* blue_o, Mat* filter) {
    int thread = 3;
    pthread_t threads[thread];
    Matrixs matrix[ ] = {{red, red_o, filter}, 
                         {green, green_o, filter}, 
                         {blue, blue_o, filter}};
    for (int i = 0; i < thread; i++) {
        pthread_create(&threads[i], NULL, &helper, (void*) &matrix[i]);
    }
    for (int i = 0; i < thread; i++)
        pthread_join(threads[i], NULL);
}

void* conv_3x3_in_color_helper(void* arg) {
    Matrixs *per = (Matrixs*) arg;
    Mat* red = per->to;
    Mat* filter = per->filter;

    int m = red->m;
    int n = red->n;
    double *lastR = (double*) calloc(m, sizeof(double));
    double *currentR = (double*)calloc(m, sizeof(double));

    double* plastR = lastR;
    double* pcurrentR = currentR;

    for (int row = 0; row < n; row++) {
        for (int column = 0; column < m; column++) {
            double sumR = 0;

          
            //index corner 1
            if (column != 0 && row != 0) {
                sumR += filter->ptr[0] * plastR[column - 1];
            }

            if (row != 0) {
                sumR += filter->ptr[1] * plastR[column];
            }
            
            
            if (column != m-1  && row != 0) {
                sumR += filter->ptr[2] * plastR[column + 1];
            }
           
            if (column != 0) {
                sumR += filter->ptr[3] * pcurrentR[column - 1];
            }

            
           if (column != m - 1) {
                sumR += filter->ptr[5] * red->ptr[row *m+ (column + 1)];
            }

            if (row != n - 1 && column != 0) {
                sumR += filter->ptr[6] * red->ptr[(row+ 1) *m+ column - 1];
            }
            
            
            if (row != n - 1) {
                sumR += filter->ptr[7] * red->ptr[(row + 1) * m + column];
            }
            
            if (row != n - 1 && column != m - 1) {
                sumR += filter->ptr[8] * red->ptr[(row + 1) * m + column + 1];
            }
            
            pcurrentR[column] = red->ptr[row * m + column];
            red->ptr[row* m + column] *= filter->ptr[4];
            red->ptr[row* m + column] += sumR;
            if (red->ptr[row * m + column] < 0) {
                red->ptr[row * m + column] = 0;
            }
            if (red->ptr[row * m + column] > 255) {
                red->ptr[row * m + column] = 255;
            }
        }
        swapArray(&plastR, &pcurrentR);
    }
    pthread_exit(NULL);
}

void conv_3x3_in_color_multi(Mat* red, Mat* green, Mat* blue, Mat* filter){
    int thread = 3;
    pthread_t threads[thread];
    Matrixs matrix[ ] = {{red, NULL ,filter}, 
                         {green, NULL ,filter}, 
                         {blue, NULL ,filter}};
    for (int i = 0; i < 3; i++)
        pthread_create(&threads[i], NULL, &conv_3x3_in_color_helper, (void*) &matrix[i]);
    for (int i = 0; i < thread; i++)
        pthread_join(threads[i], NULL);
}

void mat_equal(Mat *mat1, Mat *mat2) {

    unsigned int i, j;
    unsigned int m1 = mat1->m, n1 = mat1->n;
    unsigned int m2 = mat2->m, n2 = mat2->n;
    if (m1 != m2 || n1 != n2) {
        printf("not equal \n");
    }

    for (int i = 0; i < m1 * n2; i++) {
        if (mat1->ptr[i] != mat2->ptr[i]) {
            printf("not equal \n");
            return;
        }
    }
    printf("equal \n");
    return;
}

struct argument {
    Mat* red;
    Mat* green;
    Mat* blue;
    Mat* filter;
} argument;

int index;
pthread_barrier_t our_barrier;
void* helper_data(void* ptr){
    struct argument* arg = (struct argument*) ptr;
    Mat* red = arg->red;
    Mat* green = arg->green;
    Mat* blue = arg->blue;
    Mat* filter = arg->filter;
    int n = red->n;
    int m = red->m;
    unsigned start = __atomic_fetch_add(&index, 1, __ATOMIC_SEQ_CST);
    int end;
    if(start == omp_thread_count -1){
        start*=red->n/omp_thread_count;
        end = red->n;
    }else{
        start*=red->n/omp_thread_count;
        end = start+ (red->n/omp_thread_count);
    }
    double *lastR = (double*) calloc(m, sizeof(double));
    double *currentR = (double*)calloc(m, sizeof(double));

    double* lastG = (double*)calloc(m, sizeof(double));
    double* currentG = (double*)calloc(m, sizeof(double));

    double* lastB = (double*)calloc(m, sizeof(double));
    double* currentB = (double*)calloc(m, sizeof(double));


    double* plastR = lastR;
    double* pcurrentR = currentR;

    double* plastG = lastG;
    double* pcurrentG = currentG;

    double* plastB = lastB;
    double* pcurrentB = currentB;

    double *lastRowR = (double*)calloc(m, sizeof(double));
    double *lastRowG = (double*)calloc(m, sizeof(double));
    double* lastRowB = (double*)calloc(m, sizeof(double));
    //assign first and last
    if(start!= 0){
        for (int column = 0; column < m; column++) {
            plastR[column] = red->ptr[(start - 1) * m + column];
            plastG[column] = green->ptr[(start -1) * m + column];
            plastB[column] = blue->ptr[(start - 1) * m + column];
        }
    }

    if(end != red->m){
        for (int column = 0; column < m; column++) {
            lastRowR[column] = red->ptr[(end) * m + column];
            lastRowG[column] = green->ptr[(end) * m + column];
            lastRowB[column] = blue->ptr[(end) * m + column];
        }
    }
    pthread_barrier_wait(&our_barrier);
    for (int row = start; row < end-1 ; row++) {
        for (int column = 0; column < m; column++) {
            double sumR = 0;
            double sumG = 0;
            double sumB = 0;

            //index corner 1
            if (column != 0 && row != 0) {
                sumR += filter->ptr[0] * plastR[column - 1];
                sumG += filter->ptr[0] * plastG[column - 1];
                sumB += filter->ptr[0] * plastB[column - 1];
            }

            if (row != 0) {
                sumR += filter->ptr[1] * plastR[column];
                sumG += filter->ptr[1] * plastG[column];
                sumB += filter->ptr[1] * plastB[column];
            }
            
            
            if (column != m-1  && row != 0) {
                sumR += filter->ptr[2] * plastR[column + 1];
                sumG += filter->ptr[2] * plastG[column + 1];
                sumB += filter->ptr[2] * plastB[column + 1];
            }
           
            if (column != 0) {
                sumR += filter->ptr[3] * pcurrentR[column - 1];
                sumG += filter->ptr[3] * pcurrentG[column - 1];
                sumB += filter->ptr[3] * pcurrentB[column - 1];
            }

            
           if (column != m - 1) {
                sumR += filter->ptr[5] * red->ptr[row *m+ (column + 1)];
                sumG += filter->ptr[5] * green->ptr[row *m+ (column + 1)];
                sumB += filter->ptr[5] * blue->ptr[row *m+ (column + 1)];
            }

            if (row != n - 1 && column != 0) {
                sumR += filter->ptr[6] * red->ptr[(row+ 1) *m+ column - 1];
                sumG += filter->ptr[6] * green->ptr[(row+ 1) *m+ column - 1];
                sumB += filter->ptr[6] * blue->ptr[(row+ 1) *m+ column - 1];
            }
            
            
            if (row != n - 1) {
                sumR += filter->ptr[7] * red->ptr[(row + 1) * m + column];
                sumG += filter->ptr[7] * green->ptr[(row + 1) * m + column];
                sumB += filter->ptr[7] * blue->ptr[(row + 1) * m + column];
            }
            
            if (row != n - 1 && column != m - 1) {
                sumR += filter->ptr[8] * red->ptr[(row + 1) * m + column + 1];
                sumG += filter->ptr[8] * green->ptr[(row + 1) * m + column + 1];
                sumB += filter->ptr[8] * blue->ptr[(row + 1) * m + column + 1];
            }
            
            pcurrentR[column] = red->ptr[row * m + column];
            red->ptr[row* m + column] *= filter->ptr[4];
            red->ptr[row* m + column] += sumR;
            if (red->ptr[row * m + column] < 0) {
                red->ptr[row * m + column] = 0;
            }
            if (red->ptr[row * m + column] > 255) {
                red->ptr[row * m + column] = 255;
            }
            pcurrentG[column] = green->ptr[row* m + column];
            green->ptr[row* m + column] *= filter->ptr[4];
            green->ptr[row* m + column] += sumG;
            if (green->ptr[row * m + column] < 0) {
                green->ptr[row * m + column] = 0;
            }
            if (green->ptr[row * m + column] > 255) {
                green->ptr[row * m + column] = 255;
            }

            pcurrentB[column] = blue->ptr[row* m + column];
            blue->ptr[row* m + column] *= filter->ptr[4];
            blue->ptr[row* m + column] += sumB;
            if (blue->ptr[row * m + column] < 0) {
                blue->ptr[row * m + column] = 0;
            }
            if (blue->ptr[row * m + column] > 255) {
                blue->ptr[row * m + column] = 255;
            }
        }
        swapArray(&plastR, &pcurrentR);
        swapArray(&plastG, &pcurrentG);
        swapArray(&plastB, &pcurrentB);
    }

    end--;
    for (int column = 0; column < m; column++) {
            double sumR = 0;
            double sumG = 0;
            double sumB = 0;
            if (column != 0 && end -1 != 0) {
                sumR += filter->ptr[0] * plastR[column - 1];
                sumG += filter->ptr[0] * plastG[column - 1];
                sumB += filter->ptr[0] * plastB[column - 1];
            }
            sumR += filter->ptr[1] * plastR[column];
            sumG += filter->ptr[1] * plastG[column];
            sumB += filter->ptr[1] * plastB[column];
        
            if (column != m-1  && end != 0) {
                sumR += filter->ptr[2] * plastR[column + 1];
                sumG += filter->ptr[2] * plastG[column + 1];
                sumB += filter->ptr[2] * plastB[column + 1];
            }
           
            if (column != 0) {
                sumR += filter->ptr[3] * pcurrentR[column - 1];
                sumG += filter->ptr[3] * pcurrentG[column - 1];
                sumB += filter->ptr[3] * pcurrentB[column - 1];
            }

            
           if (column != m - 1) {
                sumR += filter->ptr[5] * red->ptr[end *m+ (column + 1)];
                sumG += filter->ptr[5] * green->ptr[end *m+ (column + 1)];
                sumB += filter->ptr[5] * blue->ptr[end *m+ (column + 1)];
            }

            if (end != n - 1 && column != 0) {
                sumR += filter->ptr[6] * lastRowR[column - 1];
                sumG += filter->ptr[6] * lastRowG[column - 1];
                sumB += filter->ptr[6] * lastRowB[column - 1];
            }
            
            
            if (end != n - 1) {
                sumR += filter->ptr[7] * lastRowR[column];
                sumG += filter->ptr[7] * lastRowG[column];
                sumB += filter->ptr[7] * lastRowB[column];
            }
            
            if (end != n - 1 && column != m - 1) {
                sumR += filter->ptr[8] * lastRowR[column];
                sumG += filter->ptr[8] * lastRowG[column];
                sumB += filter->ptr[8] * lastRowB[column];
            }
            
            pcurrentR[column] = red->ptr[end * m + column];
            red->ptr[end* m + column] *= filter->ptr[4];
            red->ptr[end* m + column] += sumR;
            if (red->ptr[end * m + column] < 0) {
                red->ptr[end * m + column] = 0;
            }
            if (red->ptr[end * m + column] > 255) {
                red->ptr[end * m + column] = 255;
            }
            pcurrentG[column] = green->ptr[end* m + column];
            green->ptr[end* m + column] *= filter->ptr[4];
            green->ptr[end* m + column] += sumG;
            if (green->ptr[end * m + column] < 0) {
                green->ptr[end * m + column] = 0;
            }
            if (green->ptr[end * m + column] > 255) {
                green->ptr[end * m + column] = 255;
            }

            pcurrentB[column] = blue->ptr[end* m + column];
            blue->ptr[end* m + column] *= filter->ptr[4];
            blue->ptr[end* m + column] += sumB;
            if (blue->ptr[end * m + column] < 0) {
                blue->ptr[end * m + column] = 0;
            }
            if (blue->ptr[end * m + column] > 255) {
                blue->ptr[end * m + column] = 255;
            }
        }
    pthread_exit(NULL);
}


void conv_3x3_in_data_multi(Mat* red, Mat* green, Mat* blue, Mat* filter){
    pthread_t threads[omp_thread_count];
    index = 0;
    struct argument arg;
    arg.red = red;
    arg.green = green;
    arg.blue = blue;
    arg.filter = filter;
    pthread_barrier_init(&our_barrier, NULL, omp_thread_count);
    for (int i = 0; i < omp_thread_count; i++) {
        pthread_create(&threads[i], NULL, &helper_data, (void*) &arg);
    }
    for (int i = 0; i < omp_thread_count; i++)
        pthread_join(threads[i], NULL);
}

