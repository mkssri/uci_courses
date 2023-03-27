#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "vector.h" //implements

void vec_create(Vec *vec, size_t n){
    vec->n = n;
    vec->ptr = (double*)calloc(n, sizeof(double));
    if(vec->ptr == NULL){
        printf("Error: Couldn't allocate Vec.\n");
        exit(1);
    }
}

void mat_create(Mat *mat, size_t m, size_t n){
    mat->m = m;
    mat->n = n;
    mat->ptr = (double*)calloc(m*n, sizeof(double));
    if(mat->ptr == NULL){
        printf("Error: Couldn't allocate Mat.\n");
        exit(1);
    }
}

void vec_destroy(Vec *vec){
    if(vec != NULL){
        if(vec->ptr != NULL){
            free(vec->ptr);
	    vec->ptr = NULL;
        }
	vec->n = 0;
    }else
        printf("Warning: Vec already NULL.\n");
}

void mat_destroy(Mat *mat){
    if(mat != NULL){
        if(mat->ptr != NULL){
            free(mat->ptr);
            mat->ptr = NULL;
        }
	mat->m = 0;
	mat->n = 0;
    }else
        printf("Warning: Mat already NULL.\n");
}

void vec_fill(Vec *vec){
    if(vec == NULL || vec->ptr == NULL){
        printf("Error: vec_fill: vec or vec->ptr is null.");
        exit(1);
    }
    double a = 0;
    size_t i;
    for(i = 0; i<vec->n; ++i)
        vec->ptr[i] = a++;
}

void vec_fill_val(Vec *vec, double val){
    if(vec == NULL || vec->ptr == NULL){
        printf("Error: vec_fill_val: vec or vec->ptr is null.");
        exit(1);
    }
    size_t i;
    for(i = 0; i<vec->n; ++i)
        vec->ptr[i] = val;
}

void vec_fill_parts(Vec *vec, size_t first){
    if(vec == NULL || vec->ptr == NULL){
        printf("Error: vec_fill_parts: vec or vec->ptr is null.");
        exit(1);
    }
    if(vec->n < first){
        printf("Error, first part of vector bigger than vector.\n");
        exit(1);
    }
    double a = 0;
    size_t i;
    for(i=0; i<first; ++i)
        vec->ptr[i] = a++;
    a = -1;
    for(i=first; i<vec->n; ++i)
        vec->ptr[i] = a--;
}

void vec_fill_rand(Vec *vec){
    if(vec == NULL || vec->ptr == NULL){
        printf("Error: vec_fill_rand: vec or vec->ptr is null.");
        exit(1);
    }
    size_t i;
    double x, y, z, s;
    for(i=0; i<vec->n; ++i){
        x = rand();
        y = rand();
        z = rand();
        s = ((rand() % 2) - 0.5) * 2;
        vec->ptr[i] = s * x * y / z;
    }
}

void mat_fill(Mat *mat){
    if(mat == NULL || mat->ptr == NULL){
        printf("Error: mat_fill: vec or vec->ptr is null.");
        exit(1);
    }
    Vec v={.ptr=mat->ptr, .n=mat->m*mat->n};
    vec_fill(&v);
}

void mat_fill_val(Mat *mat, double val){
    if(mat == NULL || mat->ptr == NULL){
        printf("Error: mat_fill_val: mat or mat->ptr is null.");
        exit(1);
    }
    Vec v={.ptr=mat->ptr, .n=mat->m*mat->n};
    vec_fill_val(&v, val);
}

void mat_fill_diag(Mat *mat, double val){
    if(mat == NULL || mat->ptr == NULL){
        printf("Error: mat_fill_diag: mat or mat->ptr is null.");
        exit(1);
    }
    size_t off = 0, max_off;
    if(mat->m < mat->n)
        max_off = mat->m * mat->n;
    else
        max_off = mat->n * mat->n;
    for(off=0; off<max_off; off+=mat->n+1){
        mat->ptr[off] = val;
    }
    return;
}

void mat_fill_coor(Mat *mat){
    if(mat == NULL || mat->ptr == NULL){
        printf("Error: mat_fill_coor: mat or mat->ptr is null.");
        exit(1);
    }
    size_t f = 1; //smallest power of 10 bigger than n
    while(f < mat->n)
        f *= 10;
    for(size_t i=0; i<mat->m; i++)
        for(size_t j=0; j<mat->n; j++)
            mat->ptr[i*mat->n+j] = i*f+j;
}

void mat_fill_top(Mat *mat, size_t rows){
    if(mat == NULL || mat->ptr == NULL){
        printf("Error: mat_fill_top: mat or mat->ptr is null.");
        exit(1);
    }
    if(mat->m < rows){
        printf("Error: mat_fill_top: mat->m < rows.\n");
        exit(1);
    }
    size_t i;
    double A = 0, B = -1;
    for(i=0; i < mat->n*rows; ++i)
        mat->ptr[i] = A++;
    for(;i < mat->m*mat->n; ++i)
        mat->ptr[i] = B--;
}

void mat_fill_left(Mat *mat, size_t cols){
    if(mat == NULL || mat->ptr == NULL){
        printf("Error: mat_fill_left: mat or mat->ptr is null.");
        exit(1);
    }
    if(mat->n < cols){
        printf("Error: mat_fill_left mat->n < cols.\n");
        exit(1);
    }
    size_t r, j;
    double A = 0, B = -1;
    for(r=0; r < mat->m*mat->n; r+=mat->n){
        for(j=0; j<cols; ++j)
            mat->ptr[r+j] = A++;
        for(; j<mat->n; ++j)
            mat->ptr[r+j] = B--;
    }
}

void mat_fill_rand(Mat *mat){
    if(mat == NULL || mat->ptr == NULL){
        printf("Error: mat_fill_rand: mat or mat->ptr is null.");
        exit(1);
    }
    Vec v = {.ptr=mat->ptr, .n=mat->m*mat->n};
    vec_fill_rand(&v);
}

void vec_copy(Vec *to, Vec *from){
    if(from == NULL || from->ptr == NULL ||
       to == NULL || to->ptr == NULL){
        printf("Error: vec_copy: to/from or to/from->ptr is null.");
        exit(1);
    }
    size_t i;
    if(from->n != to->n){
        printf("Error: vec_copy: vectors have different size.\n");
        exit(1);
    }
    for(i=0; i<from->n; i++)
        to->ptr[i] = from->ptr[i];
}

void mat_copy(Mat *to, Mat *from){
    if(from == NULL || from->ptr == NULL ||
       to == NULL || to->ptr == NULL){
        printf("Error: mat_copy: to/from or to/from->ptr is null.");
        exit(1);
    }
    size_t size;
    if(from->m!=to->m || from->n!=to->n){
        printf("Error: mat_copy: Matrices have different size.\n");
        exit(1);
    }
    size=from->m*from->n;
    for(size_t i=0; i<size; i++)
        to->ptr[i]=from->ptr[i];
}



struct sd{
    double *ptr;
    size_t size;
};

static double alpha;
    
static void *scale(void *arg){
    double *ptr = ((struct sd*)arg)->ptr;
    size_t size = ((struct sd*)arg)->size;
    for(size_t i=0; i<size; i++)
        ptr[i] *= alpha;
    return NULL;
}

void sv_mult(double c, Vec *A, int thr){
    if(thr < 2)
        for(size_t i=0; i<A->n; i++)
            A->ptr[i] *= c;
    else{
        pthread_t thrs[thr];
        int t;
        alpha = c;
        // set part size
        struct sd sdt[thr];
        size_t part = A->n/thr;
        size_t rest = A->n%thr;
        for(t=0; t<thr; t++){
            sdt[t].size = part;
            if(rest){
                sdt[t].size += 1;
                rest -= 1;
            }
        }
        //launch threads
        double *ptr = A->ptr;
        for(t=0; t<thr; t++){
            sdt[t].ptr = ptr;
            ptr += sdt[t].size;
            pthread_create(&thrs[t], NULL, &scale, (void*)&sdt[t]);
        }
        for(t=0; t<thr; t++)
            pthread_join(thrs[t],NULL);
    }
}

void sm_mult(double c, Mat *A, int thr){
    Vec v={.ptr=A->ptr, .n=A->m*A->n};
    sv_mult(c, &v, thr);
}


void vv_mult(Vec *C, Vec *A, Vec *B){
    if(!(A->n == B->n && B->n == C->n)){
        printf("Error: vv_mult: Vector sizes do not match.\n");
        exit(1);
    }
    for(size_t i=0; i<A->n; i++){
        C->ptr[i] = A->ptr[i] * B->ptr[i];
    }
}

void vv_dot(double *c, Vec *A, Vec *B){
    if(A->n != B->n){
        printf("Error: vv_mult: Vector sizes do not match.\n");
        exit(1);
    }
    register double tmp = 0;
    for(size_t i=0; i<A->n; i++){
        tmp += A->ptr[i] * B->ptr[i];
    }
    *c = tmp;
}

void vm_mult(Vec *C, Vec *A, Mat *B){
    if(A->n != B->m || B->n != C->n){
        printf("Error: vm_mult: Vector/Matrix sizes do not match.\n");
        exit(1);
    }
    register double tmp;
    for(size_t i=0; i<B->m*B->n; i+=B->n){
        tmp = 0;
        for(size_t j=0; j<A->n; j++)
            tmp += A->ptr[j] * B->ptr[i+j*B->n];
        C->ptr[i] = tmp;
    }
}

void mv_mult(Vec *C, Mat *A, Vec *B){
    if(A->n != B->n || A->m != C->n){
        printf("Error: mv_mult: Vector/Matrix sizes do not match.\n");
        exit(1);
    }
    register double tmp;
    for(size_t i=0; i<A->m*A->n; i+=A->n){
        tmp = 0;
        for(size_t j=0; j<B->n; j++){
            tmp += A->ptr[i+j] * B->ptr[j];
        }
        C->ptr[i] = tmp;
    }
}

/**
 **        A.n k                B.n j              C.n j
 **     [ ][ ][ ][ ]      [ ][ ][ ][ ][ ]      [ ][ ][ ][ ][ ]
 ** A.m [ ][ ][ ][ ]  B.m [ ][ ][ ][ ][ ]  C.m [ ][ ][ ][ ][ ]
 **  i  [ ][ ][ ][ ]   k  [ ][ ][ ][ ][ ]   i  [ ][ ][ ][ ][ ]
 **                       [ ][ ][ ][ ][ ]
 **/
void mm_mult(Mat *C, Mat *A, Mat *B){
    if(A->n != B->m || B->n != C->n || A->m != C->m){
        printf("Error: mm_mult: Matrix sizes do not match.\n");
        exit(1);
    }
    register double tmp;
    size_t Ia, Ic, K;
    Ia = Ic = 0;
    for(size_t i=0; i<A->m; i++){
        for(size_t j=0; j<B->n; j++){
            tmp = 0;
            K = 0;
            for(size_t k=0; k<A->n; k++){
                tmp += A->ptr[Ia+k] * B->ptr[K+j];
                K += B->n;
            }
            C->ptr[Ic+j] = tmp;
        }
        Ia += A->n;
        Ic += C->n;
    }
}

/**
 **        A.n k              BT.n k              C.n j
 **     [ ][ ][ ][ ]       [ ][ ][ ][ ]      [ ][ ][ ][ ][ ]
 ** A.m [ ][ ][ ][ ]  BT.m [ ][ ][ ][ ]  C.m [ ][ ][ ][ ][ ]
 **  i  [ ][ ][ ][ ]    j  [ ][ ][ ][ ]   i  [ ][ ][ ][ ][ ]
 **                        [ ][ ][ ][ ] 
 **                        [ ][ ][ ][ ]
 **/
void mm_rmult(Mat *C, Mat *A, Mat *BT){
    if(A->n != BT->n || BT->m != C->n || A->m != C->m){
        printf("Error: mm_rmult: Matrix sizes do not match.\n");
        exit(1);
    }
    register double tmp;
    size_t Ia, Ic, J;
    Ia = Ic = 0;
    for(size_t i=0; i<A->m; i++){
        J = 0;
        for(size_t j=0; j<BT->m; j++){
            tmp = 0;
            for(size_t k=0; k<A->n; k++){
                tmp += A->ptr[Ia+k] * BT->ptr[J+k];
            }
            J += A->n;
            C->ptr[Ic+j] = tmp;
        }
        Ia += A->n;
        Ic += C->n;
    }
}


int vec_diff(Vec *v1, Vec *v2, size_t *diff){
    size_t i;
    if(v1->n != v2->n)
	return 2;
    for(i=0; i<v1->n; i++){
	if(v1->ptr[i] != v2->ptr[i]){
	    *diff = i;
	    return 1;
	}
    }
    return 0;
}

int mat_diff(Mat *m1, Mat *m2, size_t *diff){
    Vec v1={m1->ptr, m1->m*m1->n};
    Vec v2={m2->ptr, m2->m*m2->n};
    if(m1->m != m2->m || m1->n != m2->n)
	return 2;
    return vec_diff(&v1, &v2, diff);
}

void vec_print(Vec *vec){
    //size of the screen and number of boxes for the sample
    size_t h_screen=150, hboxes=3;

    //get log(max_elem) for padding
    size_t max;
    int padding=1;
    max=abs(vec->n);//huge range assumtion here
    while(max>0){
        padding += 1;
        max /= 10;
    }

    //small vector, print it all
    if(vec->n <= h_screen/padding){
        size_t i;
        for(i = 0; i<vec->n; ++i)
            printf("%*.f", padding, vec->ptr[i]);
        printf("\n");
        return;
    }

    //big vector, print a sample
    unsigned int box_h; //size of the boxes
    size_t bh_jump; //jumps between boxes
    
    box_h = (h_screen/padding - hboxes+1)/hboxes;
    box_h = box_h%2==0 ? box_h : box_h+1;
    box_h = vec->n%(hboxes-1)==0 ? box_h : box_h-1;

    bh_jump = (vec->n-box_h) / (hboxes-1);
    
    if(hboxes*box_h >= vec->n){
        hboxes=1;
        box_h=vec->n;
    }

    size_t bh, h;
    double tmp;
    for(bh=0; bh<hboxes; ++bh){
        for(h=0; h<box_h; ++h){
            tmp=vec->ptr[bh*bh_jump + h];
            printf("%*.0f", padding, tmp);
        }
        if(bh+1 < hboxes) printf("%*c", padding, ' ');
    }
    printf("\n");       
}
void mat_print(Mat *mat){
    //size of the screen and number of boxes for the sample
    size_t h_screen=150, v_screen=34;
    size_t hboxes=3, vboxes=3;

    //get log(max_elem) for padding
    size_t max;
    int padding=1;
    max=abs(mat->ptr[mat->m*mat->n-1]); //huge range assumtion here
    while(max>0){
        padding += 1;
        max /= 10;
    }

    //small matrix, print it all
    if(mat->m <= v_screen && mat->n <= (h_screen/padding)){
        //matrix is small, print it all
        size_t j, offset=0;
        while(offset < (mat->m)*mat->n){
            for(j=0; j < mat->n; ++j){
                printf("% *.0f", padding, mat->ptr[offset+j]);
            }
            printf("\n");
            offset+=mat->n;
        }
        return;
    }
    
    //big matrix, print a sample
    size_t box_h, box_v; //hor & vert size of the boxes
    size_t bh_jump, bv_jump; //hor & vert jumps between boxes
        
    box_h = (h_screen/padding - hboxes + 1) / hboxes;
    box_h = box_h%2==0 ? box_h : box_h+1;
    box_h = mat->n%(hboxes-1)==0 ? box_h: box_h-1;
    bh_jump = (mat->n-box_h) / (hboxes-1);
    if(hboxes*box_h >= mat->n){
        hboxes=1;
        box_h=mat->n;
    }

    box_v = (v_screen - vboxes+1) / vboxes;
    box_v = box_v%2==0 ? box_v : box_v+1;
    box_v = mat->m%(vboxes-1)==0 ? box_v :box_v-1;
    bv_jump = (mat->m-box_v) / (vboxes-1);      
    if(vboxes*box_v >= mat->m){
        vboxes=1;
        box_v=mat->m;
    }
    
    size_t bv, v, bh, h;
    double tmp;
    for(bv=0; bv<vboxes; ++bv){
        for(v=0; v<box_v; ++v){
            for(bh=0; bh<hboxes; ++bh){
                for(h=0; h<box_h; ++h){
                    tmp=mat->ptr[bv*bv_jump*mat->n +     \
                                 v*mat->n +              \
                                 bh*bh_jump +            \
                                 h];
                    printf("%*.0f", padding, tmp);
                }
                if(bh+1 < hboxes) printf("%*c", padding, ' ');
            }
            printf("\n");
        }
        if(bv+1 < vboxes) printf("\n");
    }
}

/* end of life */
