typedef struct {
    double* ptr; // pointer to the first element
    unsigned int m; //height
    unsigned int n; //width
} Mat;

extern void mat_init(Mat* mat, unsigned int m, unsigned int n);

extern void mat_print(Mat* mat);

extern void mat_copy(Mat* from, Mat* to);

extern void mat_equal(Mat *mat1, Mat *mat2);

extern void storeImage(Mat* red, Mat* green, Mat* blue, FILE* fp);

extern void writeImage(Mat* red, Mat* green, Mat* blue);

extern void filterAssign(Mat* mat, int option);

extern void conv_2x2(Mat* red, Mat* green, Mat* blue, Mat* filter);

extern void conv_3x3(Mat* red, Mat* green, Mat* blue, Mat* filter);

extern void conv_3x3_out(Mat* red, Mat* green, Mat* blue, Mat* red_o, Mat* green_o, Mat* blue_o, Mat* filter);

extern void conv_3x3_out_muli_data (Mat* red, Mat* green, Mat* blue, Mat* red_o, Mat* green_o, Mat* blue_o, Mat* filter);

extern void conv_3x3_out_muli_color(Mat* red, Mat* green, Mat* blue, Mat* red_o, Mat* green_o, Mat* blue_o, Mat* filter);

extern void conv_3x3_in_color_multi(Mat* red, Mat* green, Mat* blue, Mat* filter);

extern void conv_3x3_in_data_multi(Mat* red, Mat* green, Mat* blue, Mat* filter);
