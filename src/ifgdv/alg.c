// =====================================================================
// Algorithms and mathmatical stuff
// (c) - 2013 A. Weidauer  alex.weidauer@huckfinn.de
// All rights reserved to A. Weidauer
// =====================================================================
#include "ifgdv/alg.h"

// ---------------------------------------------------------------
void error_exit(int code, const char *message, ...) {
    va_list arglist;
    va_start(arglist,message);
    vfprintf(stderr, message, arglist );
    va_end(arglist);
    exit(code);
}

// ---------------------------------------------------------------
int trfm_create(dbl_vector_t *src_x,
        dbl_vector_t *src_y,
        dbl_vector_t *dst_x,
        dbl_vector_t *dst_y,
        double *result) {

    // minimal number of points avialable
    if (src_x->length < 3) return -1;

    // vector length is equal for sourc an destination
    if (src_x->length != src_y->length ||
            dst_x->length != dst_y->length ||
            src_x->length != dst_x->length ||
            src_y->length != dst_y->length)
        return -2;

    // init matrices and vectors
    gsl_matrix *M = gsl_matrix_alloc(3, 3);
    gsl_vector *B1 = gsl_vector_alloc(3);
    gsl_vector *B2 = gsl_vector_alloc(3);
    gsl_vector *CX = gsl_vector_alloc(3);
    gsl_vector *CY = gsl_vector_alloc(3);

    // fill zeros
    gsl_matrix_set_zero(M);
    gsl_vector_set_zero(B1);
    gsl_vector_set_zero(B2);
    gsl_vector_set_zero(CX);
    gsl_vector_set_zero(CY);

    // build equations
    for (int i = 0; i < src_x->length; i++) {

        // normal form of sources 1st part
        M->data[0 * M->tda + 0] += src_x->data[i] * src_x->data[i];
        M->data[0 * M->tda + 1] += src_x->data[i] * src_y->data[i];
        M->data[0 * M->tda + 2] += src_x->data[i];

        // normal form of sources 2nd part
        M->data[1 * M->tda + 0] += src_x->data[i] * src_y->data[i];
        M->data[1 * M->tda + 1] += src_y->data[i] * src_y->data[i];
        M->data[1 * M->tda + 2] += src_y->data[i];

        // mixed terms 1 source destination points
        B1->data[0 * B1->stride] += dst_x->data[i] * src_x->data[i];
        B1->data[1 * B1->stride] += dst_x->data[i] * src_y->data[i];
        B1->data[2 * B1->stride] += dst_x->data[i];

        // Mmxed terms 2 source destination points
        B2->data[0 * B2->stride] += dst_y->data[i] * src_x->data[i];
        B2->data[1 * B2->stride] += dst_y->data[i] * src_y->data[i];
        B2->data[2 * B2->stride] += dst_y->data[i];
    }

    // setup remaining terms int normal form
    gsl_matrix_set(M, 2, 2, src_x->length);
    gsl_matrix_set(M, 2, 1, gsl_matrix_get(M, 1, 2));
    gsl_matrix_set(M, 2, 0, gsl_matrix_get(M, 0, 2));

    // solve the equations
    int s = 0;
    gsl_permutation * P = gsl_permutation_alloc(3);
    gsl_linalg_LU_decomp(M, P, &s);

    gsl_linalg_LU_solve(M, P, B1, CX);
    gsl_linalg_LU_solve(M, P, B2, CY);

    // free vactors an matrices
    gsl_matrix_free(M);
    gsl_vector_free(B1);
    gsl_vector_free(B2);
    gsl_permutation_free(P);

    // write resuling vectors
    for (int i = 0; i < 3; ++i) {
        result[i] = gsl_vector_get(CX, i);
        result[i + 3] = gsl_vector_get(CY, i);
    }
    // free intermediate vectors
    gsl_vector_free(CX);
    gsl_vector_free(CY);
}

// -------------------------------------------------------------------
void sort_int(int * a, int * b) {
  if ( *a > *b)
  {
     int t = *a;
     *a = *b;
     *b = t;
  }
}

// -------------------------------------------------------------------
char *strip_ext(char *txt) {
	size_t len = strlen(txt);
	if (len==0) return(txt);
	for (int l=len; l>0; l--) {
		if (txt[l]=='.') {
			txt[l] = '\0';
			return(txt);
		}
	}
	return(txt);
}
// -------------------------------------------------------------------
void trfm_pix_geo(const double *trfm,
                  double col, double row,
                  double *x , double *y) {
  *x = trfm[0] + trfm[1] * col + trfm[2] * row;
  *y = trfm[3] + trfm[4] * col + trfm[5] * row;
}


// -------------------------------------------------------------------
int trfm_geo_pix(const double *trfm,
                double x, double y,
                long *col , long *row) {

  double div = (trfm[2]*trfm[4]-trfm[1]*trfm[5]);
  if (div < DBL_EPSILON * 2) return 0;
  double dcol = -(trfm[2]*(trfm[3]-y)+trfm[5]*x-trfm[0]*trfm[5])/div;
  double drow =  (trfm[1]*(trfm[3]-y)+trfm[4]*x-trfm[0]*trfm[4])/div;
  *col = round(dcol); *row = round(drow);
  return 1;
}

// ---------------------------------------------------------------
void dbl_vector_init(dbl_vector_t *vec, size_t size) {
    vec->data = (double *) malloc(size * sizeof (double));
    vec->length = 0;
    vec->mem_size = size;
}

// ---------------------------------------------------------------
void dbl_vector_add(dbl_vector_t *vec, double element) {
    if (vec->length == vec->mem_size) {
        vec->mem_size += vec->mem_size;
        vec->data = (double *) realloc(vec->data,
                                       vec->mem_size * sizeof (double));
    }
    vec->data[vec->length++] = element;
}

// ---------------------------------------------------------------
void dbl_vector_free(dbl_vector_t *vec) {
    free(vec->data);
    vec->data = NULL;
    vec->length = vec->mem_size = 0;
}

// ---------------------------------------------------------------
void int_vector_init(int_vector_t *vec, size_t size) {
    vec->data = (int *) malloc(size * sizeof (int));
    vec->length = 0;
    vec->mem_size = size;
}

// ---------------------------------------------------------------
void int_vector_add(int_vector_t *vec, int element) {
    if (vec->length == vec->mem_size) {
        vec->mem_size += vec->mem_size;
        vec->data = (int *) realloc(vec->data,
                                    vec->mem_size * sizeof (int));
    }
    vec->data[vec->length++] = element;
}

// ---------------------------------------------------------------
void int_vector_free(int_vector_t *vec) {
    free(vec->data);
    vec->data = NULL;
    vec->length = vec->mem_size = 0;
}


// --- EOF ---------------------------------------------------------
