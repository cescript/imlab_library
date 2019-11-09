#ifndef __IMLAB_PRCORE_H__
#define __IMLAB_PRCORE_H__

// Include C99 libraries
#include <omp.h>
#include <math.h>
// Include IMLAB general headers
#include "core.h"

/**
Maximum number can be genarted by IMLAB Random class_name
*/
#define random_max 4294967295 // 2**32-1
/**
Initial seed for the random number generator.
*/

// general probability and statistic functions
int32_t *range(int32_t start, int32_t end);
// return pearson correlation coefficient of two vector
float pearson(const float *vecA, const float *vecB, int length);
float covariance(const float *vecA, const float *vecB, int length);

/**
 * Create a random seed for the random number generator.
 */
void random_seed();
/**
Set the seed for the random number generator. This is useful when you need to generate controlled random numbers. Avoid using 0 as the initial seed.
*/
void random_setseed(uint32_t state[4]);
/**
Get the current seed state to replicate PRNG process later.
*/
void random_getseed(uint32_t state[4]);
/**
Wikipedia implementation of xorshift128 algorithm. This will produce a random integer in \f$[0,2^32-1]\f$ range with a period of \f$2^128-1\f$.
Which is quite unpredictable and random for most of the applications.
*/
uint32_t random();
/**
Generate a random integer in [mint, maxt] range with a uniform distribution.
*/
int random_int(int mint, int maxt);
// uniform random float between given range
float random_float(float mint, float maxt);

// int *perm = (int*) malloc(N*sizeof(N));
uint32_t *random_permutation(uint32_t length);

void *random_sample(uint8_t *in, uint32_t length, uint32_t sample, uint32_t elemsize);

#endif