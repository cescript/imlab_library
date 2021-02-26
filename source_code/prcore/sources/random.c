/**
\file random.c
Imlab uses XOR Shift methodin order to create a random number. It supports the following functions
   - random() : returns a uniform random variable in [0,1] range
   - random_float(low, high) : return a uniform random floating variable in [low, high] range
   - random_int(low, high) : return a uniform integer in [low, high] range
   - random_permutation(length) : return an uint32_t* array which is permutation of indices between [0, length-1]. This can be used to shuffle the vectors or random sampling.
*/
/* Implemented in future
// random_fill(mat, low, high) : fill a matrix with random variable
// random_normal(mean, std) : return a normal random variable
// random_shuffle(vector) : shuffle the entries of the vector withou generating index list
// general probability and statistic functions

/// RANDOM CLASS: Definitions of various utility functions
//  random()            : returns 32 bit unsigned random value (similar to rand)
//  random_max          : maximum of the generated random value (2**32-1)
//  random_int          : generates signed integer number between [min,max]
//  random_float        : generates floating point number between [min,max]
//  random_permutation  : creates suffled array of inetegers in [0,N] range
//  random_suffle       : suffles the given input array
*/
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "private/prcore_private.h"

uint32_t imlab_random_seed[4] = {123456789,362436069,521288629,88675123};

// randomly generate seed
void random_seed()
{
    // get the current time and use it as the seed
    srand(time(NULL));

    // fill the random number generators seed
    imlab_random_seed[0] = (uint32_t )rand();
    imlab_random_seed[1] = (uint32_t )rand();
    imlab_random_seed[2] = (uint32_t )rand();
    imlab_random_seed[3] = (uint32_t )rand();
}

void random_setseed(uint32_t state[4])
{

    check_condition(state[0]==state[1]==state[2]==state[3] == 0,,  "cannot set all the seed states to zero at the same time");

    imlab_random_seed[0] = state[0];
    imlab_random_seed[1] = state[1];
    imlab_random_seed[2] = state[2];
    imlab_random_seed[3] = state[3];
}
/**
Get the current seed state to replicate PRNG process later.
*/
void random_getseed(uint32_t state[4])
{
    state[0] = imlab_random_seed[0];
    state[1] = imlab_random_seed[1];
    state[2] = imlab_random_seed[2];
    state[3] = imlab_random_seed[3];
}
/**
Wikipedia implementation of xorshift128 algorithm. This will produce a random integer in [0,2**32-1] range with a period of 2**128−1.
Which is quite unpredictable and random for most of the applications.
*/
uint32_t random()
{
	uint32_t t = imlab_random_seed[3];
	t ^= t << 11;
	t ^= t >> 8;
	imlab_random_seed[3] = imlab_random_seed[2];
    imlab_random_seed[2] = imlab_random_seed[1];
    imlab_random_seed[1] = imlab_random_seed[0];
	t ^= imlab_random_seed[0];
	t ^= imlab_random_seed[0] >> 19;
	imlab_random_seed[0] = t;
    // done
	return t;
}

/**
Generate a random integer in [mint, maxt] range with a uniform distribution.
*/
int random_int(int mint, int maxt)
{
    // create random int between [0, mx-mn] and add mn to it
    return mint + (random() % (maxt-mint+1));
}

// uniform random float between given range
float random_float(float mint, float maxt)
{
    // create random int between [0, mx-mn] and add mn to it
    double zero_one = random() / (double) random_max;
    return (float) (mint + zero_one*(maxt-mint));
}

// int *perm = (int*) malloc(N*sizeof(N));
uint32_t *random_permutation(uint32_t length)
{

    // if the input is empty we cannot return anything
    if(length < 1) {
        return NULL;
    }
    // otherwise we can compute the new vector
    uint32_t i, j, temp;

    uint32_t *idx_list = (uint32_t*) malloc(length*sizeof(uint32_t));
    // initialize array idx_list[i] = i
    for(i=0; i < length; i++) { idx_list[i] = i; }

    // do Fisher Yates O(n) shuffle
    for( i=0; i < length-1; i++) {
        //j <- random integer such that i < j < n
        j = random_int(i+1, length-1);
        // swap the perm[i] and perm[j]
        temp = idx_list[i];
        idx_list[i] = idx_list[j];
        idx_list[j] = temp;
    }
    // done
    return idx_list;
}

// implement simple reservoir sampling strategy to select sample over length
return_t random_sample(void *input, uint32_t length, uint32_t sample, uint32_t elemsize, void *output)
{
    // check that the input and output arrays are allocated
    check_null(input, ERROR_NULL_TYPE);
    check_null(output, ERROR_NULL_TYPE);

    // check that the array length is larger than the sample size
    check_condition( sample, ERROR_DIMENSION_MISMATCH, "input length must be larger than the sample size");

    // get the iterable pointer to the actual data
    uint8_t *in = input;
    uint8_t *out = output;

    // first fill the array with the first values
    uint32_t s;
    for(s = 0; s < sample; s++)
    {
        memcpy(out + s*elemsize, in, elemsize);
        in += elemsize;
    }

    // check the remaining samples and select if probability confirms
    for(; s < length; s++) 
    {
        // get the accaptance threshold
        uint32_t r = (uint32_t)random_int(0, s);

        // accept the input if the index is smaller than the sample size
        if(r < sample) 
        {
            memcpy(out + r*elemsize, in, elemsize);
        }

        // make the input points to the next element
        in += elemsize;
    }

    // return success
    return SUCCESS;
}