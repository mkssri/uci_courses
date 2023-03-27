#include "transpose.h" //implements
#include "shuffle.h"
#include "square-transp.h"


void transpose(double **mat, size_t rows, size_t cols, size_t thr){
    double *rem_mat;
    if(rows < 2  || cols < 2)
        //base case: either a 1D vector, or empty array.
        return;
    if(rows == cols){
        //matrix is square:
        // - base case.
        // - transpose it.
        transpose_square(*mat, rows, thr);
    }else if(rows < cols){
        //matrix is wide:
        // - unshuffle the parts apart
        // - transpose the left square
        // - transpose the right remaindig
        unshuffle(*mat, rows, rows, cols-rows, thr);
        transpose_square(*mat, rows, thr);
        rem_mat = *mat+rows*rows;
        transpose(&rem_mat, rows, cols-rows, thr);
    }else if(rows > cols){
        //matrix is tall:
        // - transpose top square
        // - transpose bottom remainding
        // - shuffle the parts back together
        transpose_square(*mat, cols, thr);
        rem_mat = *mat+cols*cols;
        transpose(&rem_mat, rows-cols, cols, thr);
        shuffle(*mat, cols, cols, rows-cols, thr);
    }
}
