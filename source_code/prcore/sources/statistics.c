#include <stdlib.h>
#include "private/prcore_private.h"

int32_t *range(int32_t start, int32_t end) {
    // allocate memory for the output
    int32_t *out = malloc( (end-start+1) * sizeof(int32_t));
    // fill the memory
    uint32_t i;
    for(i=0; i < end-start+1; i++) {
        out[i] = start + i;
    }
    return out;
}

// return pearson correlation coefficient of two vector
float pearson(const float *vecA, const float *vecB, int length) {
    int i;

    double mu_va = 0, mu2_va = 0;
    double mu_vb = 0, mu2_vb = 0;
    double va_vb = 0;

    for(i=0; i < length; i++) {

        double a = vecA[i];
        double b = vecB[i];

        mu_va  += a;
        mu_vb  += b;
        mu2_va += a*a;
        mu2_vb += b*b;
        va_vb  += a*b;
    }

    mu_va = mu_va   / length;
    mu_vb = mu_vb   / length;
    mu2_va = mu2_va / length;
    mu2_vb = mu2_vb / length;
    va_vb  = va_vb  / length;

    double divsr = (mu2_va-mu_va*mu_va)*(mu2_vb-mu_vb*mu_vb);

    if(equal(divsr, 0, imlab_epsilon)) { return 0; }
    // return the correlation
    return (float)((va_vb - mu_va*mu_vb) / (sqrt(divsr) ));
}

// return pearson correlation coefficient of two vector
float covariance(const float *vecA, const float *vecB, int length) {
    int i;

    double mu_va = 0, mu2_va = 0;
    double mu_vb = 0, mu2_vb = 0;
    double va_vb = 0;

    for(i=0; i < length; i++) {

        double a = vecA[i];
        double b = vecB[i];

        mu_va  += a;
        mu_vb  += b;
        va_vb  += a*b;
    }
    // compute the Eab, Ea and Eb and return Eab-Ea*Eb
    mu_va = mu_va   / length;
    mu_vb = mu_vb   / length;
    va_vb  = va_vb  / length;
    // return the correlation
    return (float)((va_vb - mu_va*mu_vb));
}