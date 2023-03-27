#include "transpose.h"
#include "vector.h"
#include <pthread.h>
//#include <stdio.h>

size_t EXCH_MIN_GRAIN; // unused here
size_t SQTR_MIN_GRAIN; // unused here

typedef struct thr_data
{
	double* mat;
	unsigned begin;
	unsigned delta;
	unsigned h;
	unsigned w;
} ThrData;

//-----------------------------------------------------------------------------
void MatrixTransposeCycles(unsigned begin, unsigned delta, const unsigned h, const unsigned w, double m[])
{
	for (unsigned start = begin; start <= w * h - 1; start += delta)
	{
		unsigned next = start;
		unsigned i = 0;
		do
		{
			++i;
			next = (next % h) * w + next / h; //convert the position between the original and the transposed matrix
		} while (next > start);
		/*
		 * conditions:
		 * 1. if sequences of length 1, no required data motion and can be skipped
		 * 2. if start is smaller than next, then it must be a subsequence and can be skipped.
		 */
		if (next >= start && i != 1)
		{
			const double tmp = m[start];
			next = start;
			do
			{
				i = (next % h) * w + next / h;
				m[next] = (i == start) ? tmp : m[i];
				next = i;
			} while (next != start);
		}
	}
}



//-----------------------------------------------------------------------------

void *thread_work(void *arg){
	ThrData *data = (ThrData*) arg;
	MatrixTransposeCycles(data->begin, data->delta, data->h, data->w, data->mat);
	pthread_exit(NULL);
}

//-----------------------------------------------------------------------------

void transpose(double **mat, size_t rows, size_t cols, size_t thr){
    if(thr < 2){
        MatrixTransposeCycles(0, 1, rows, cols, *mat);
        return;
    }
	size_t i;
	pthread_t threads[thr];
	ThrData thr_data[thr];
	for (i = 0; i < thr; ++i){
		thr_data[i].begin = i;
		thr_data[i].delta = thr;
		thr_data[i].mat = *mat;
		thr_data[i].h = rows;
		thr_data[i].w = cols;
	}
	for (i = 0; i < thr; ++i)
		pthread_create(&threads[i], NULL, thread_work, (void*)&thr_data[i]);
	for (i = 0; i < thr; ++i)
		pthread_join(threads[i],NULL);
    return;
}

