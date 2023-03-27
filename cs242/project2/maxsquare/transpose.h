typedef struct {
    int *ptr; // pointer to the first element
    unsigned int m; //height
    unsigned int n; //width
} Mat;

/**
** Randomize's a square matrix 
**/
extern void randomize(Mat *mat);

/**
 ** Allocates @m*@n elements of memory for matrix @mat
 **/
extern void mat_init(Mat *mat, unsigned int m, unsigned int n);

/**
 ** Fill matrix @mat with consecutive values.
 **/
extern void mat_fill(Mat *mat);

/**
 ** Fill diagonal of matrix @mat with @diag.
 **/
extern void mat_diag(Mat *mat, double diag);

/**
 ** Copies matrix @from to matrix @to. Matrix @to has to be
 ** previously initialized.
 **/
extern void mat_copy(Mat *to, Mat *from);

/**
 ** Print @mat in @mat->m rows, @mat->n columns
 **/
extern void mat_print(Mat *mat);

extern void mat_equal(Mat *mat1, Mat *mat2);

extern void naive_mat_transpose(Mat *mat);

extern void mat_transpose(Mat *mat);

extern void MTCycles(const unsigned rows, const unsigned columns, Mat *mat);

extern void Updated_MTCycles(const unsigned rows, const unsigned columns, Mat *mat);

extern void SquareMultithread(const unsigned rows, const unsigned columns, Mat *mat);

extern void RectangularMultithread(const unsigned rows, const unsigned columns, Mat *mat);

extern void transpose_block(Mat *A, Mat *B,  const int block_size);

extern void transpose_scalar_block(int *A, int *B, const int lda, const int ldb, const int block_size);

extern void smallMulTranspose(Mat *mat, Mat *mat2);

extern void InplaceMulti(Mat *A);
