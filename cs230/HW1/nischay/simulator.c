#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

double rand_gen() {
   // return a uniformly distributed random value
   double randNum = rand();
//    printf("rand: %lf\n", randNum);
   return randNum/( (double)(RAND_MAX) + 1. );
}

int uniform_distribution(int rangeLow, int rangeHigh) {
    double myRand = rand()/(1.0 + RAND_MAX); 
    int range = rangeHigh - rangeLow + 1;
    int myRand_scaled = (myRand * range) + rangeLow;
    return myRand_scaled;
}

double randN(double mean, double std, FILE *fp) {
    double x1 = rand_gen();
    double x2 = rand_gen();

    double randN = cos(2 * M_PI * x2)*sqrt(-2.0 * log(x1));
    // printf("std: %lf mean: %lf\n", std, mean);
    fprintf(fp, "%lf\n", randN*std + mean);
    // return (int) round(randN * std + mean);
}

int main() {
    srand(time(NULL));
    FILE *fp;
    // printf("%f\n", M_PI);
    fp = (FILE*) fopen("uniform_data.txt", "w");
    for(int i = 0; i < 200000; i++) {
        double num = randN(2.0, 2.0/6, fp);
        // int num = uniform_distribution(0, 2048);
        // int num = rand()%2048;
        // fprintf(fp, "%lf\n", num);
        // printf("%lf\n", num);
    }
    fclose(fp);
}