// Fast Dot Product of two vectors r := xy
float fdot1(const float  *x, const float  *y, const int length) {

    int i;
    float result = 0.0;
    // use just one loop and compute dot product
    for (i=0; i < length; i++) {
        result += x[i] * y[i];
    }
    return result;
}
// Fast Dot Product of two vectors r := xy
#define fdot4(type, x,y, result, length) \
    do{\
        int i;\
        type r0 = 0, r1 = 0, r2 = 0, r3 = 0;\
        for (i=0; i < length; i+=4) {\
            r0 += (x)[i+0] * (y)[i+0];\
            r1 += (x)[i+1] * (y)[i+1];\
            r2 += (x)[i+2] * (y)[i+2];\
            r3 += (x)[i+3] * (y)[i+3];\
        }\
        for(i; i < length; i++) {\
            r3 += (x)[i] * (y)[i];\
        }\
        (result) = (r0+r1+r2+r3);\
    } while(0);

/*
float fdot4(const float  *x, const float  *y, const int length) {

    int i;
    float r0=0.0f, r1=0.0f, r2=0.0f, r3=0.0f;
    // partition the array into blocks for vectorization
    for (i=0; i < length; i+=4) {
        r0 += x[i+0] * y[i+0];
        r1 += x[i+1] * y[i+1];
        r2 += x[i+2] * y[i+2];
        r3 += x[i+3] * y[i+3];
    }
    for(i; i < length; i++) {
        r3 += x[i] * y[i];
    }

    return (r0+r1+r2+r3);
}
*/

/*
// Fast Matrix Scalar Multiplacation X := alpha*X
void dgescal(double  **X, double  alpha, int m, int n)
{
    int i;
		// alpha == 1 -> no change
		if (is_equal(alpha,1.0, IM_EPS)) { return; }
		// alpha == 0 -> all entries will be zero
    if (is_equal(alpha,0.0, IM_EPS)) {
			memset( (*X), 0, m*n*sizeof(double) );
			return;
		}
		// Otherwise X := X*alpha
    for (i=0; i<m*n; ++i) { (*X)[i] *= alpha; }
}
//  Compute Y += alpha*X, we can compute r := X+Y, XT+Y, alpha*X+YT, alpha*XT+YT,...
void eaxpy(const double  *X, double alpha, double **Y, int m, int n, int incRowX, int incColX, int incRowY, int incColY)
{
    int i, j;
		// alpha == 0 -> no change on Y
		if (is_equal(alpha,0.0, IM_EPS)) { return; }
		// alpha == 1 -> Y += X
    if (is_equal(alpha,1.0, IM_EPS)) {
        for (j=0; j<n; ++j) {
            for (i=0; i<m; ++i) {
                (*Y)[i*incRowY+j*incColY] += X[i*incRowX+j*incColX];
            }
        }
    } else {
        for (j=0; j<n; ++j) {
            for (i=0; i<m; ++i) {
                (*Y)[i*incRowY+j*incColY] += alpha*X[i*incRowX+j*incColX];
            }
        }
    }
}
*/
