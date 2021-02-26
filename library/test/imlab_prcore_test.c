#include <stdlib.h>

#include "core.h"
#include "prcore.h"

/* LIST OF ALL FUNCTIONS */
/*
int32_t *range(int32_t start, int32_t end);

float pearson(const float *vecA, const float *vecB, int length);
float covariance(const float *vecA, const float *vecB, int length);

void random_seed();
void random_setseed(uint32_t state[4]);
void random_getseed(uint32_t state[4]);

uint32_t random();
int random_int(int mint, int maxt);
float random_float(float mint, float maxt);

uint32_t *random_permutation(uint32_t length);
return_t random_sample(void *input, uint32_t length, uint32_t sample, uint32_t elemsize, void *output)
*/


int main() 
{
    // print the start message
    message(SUCCESS, "starting to test IMLAB prcore functions");

    // test range method
    {
        int32_t start = -100, end = 100, i = 0;
        int32_t length = end - start + 1;

        // create the range
        int32_t *r = range(start, end);

        for(i = 0; i < length; i++)
        {
            if( (i+start) != r[i] )
            {
                break;
            }
        }

        // free memory
        free(r);

        if(i != length)
        {
            message(ERROR, "test of the range function failed");
            return -1;
        }
    }

    // test pearson and covariance
    {
        // create sample data which has corr=1, cov=0.8286
        float v1[8] = {-2,+2, -1,+1,-0.8,+0.8, -0.4, +0.4};
        float v2[8] = {-1,+1, -0.5,+0.5,-0.4,+0.4, -0.2, +0.2};

        // test the pearson correlation coefficient
        float p = pearson(v1, v2, 8);
        if(!equal(p, 1.00000, 1e-5))
        {
            message(ERROR, "test of the pearson function failed");
            return -1;
        }

        // test the covariance method
        float c = covariance(v1, v2, 8);
        if(!equal(c, 0.72500, 1e-5))
        {
            message(ERROR, "test of the covariance function failed");
            return -1;
        }

    }

    // test random number seeds
    {
        // space for 128 bit seed
        uint32_t state[2][4];

        // get the seed
        random_getseed(state[0]);

        // randomly create a seed
        random_seed();

        // get the seed
        random_getseed(state[1]);

        // check that states are different
        if(state[0][0] == state[1][0] && state[0][1] == state[1][1] && state[0][2] == state[1][2] && state[0][3] == state[1][3])
        {
            message(ERROR, "test of the random_seed or random_getseed function failed");
            return -1;
        }

        // set the seed
        random_setseed(state[0]);

        // check that states are different
        if(state[0][0] == state[1][0] && state[0][1] == state[1][1] && state[0][2] == state[1][2] && state[0][3] == state[1][3])
        {
            message(ERROR, "test of the random_setseed function failed");
            return -1;
        }
    }

    // test random number generation
    {
        // test two randoms are different
        uint32_t r1 = random();
        uint32_t r2 = random();

        // check that numbers are different
        if(r1 == r2)
        {
            message(ERROR, "test of the random function failed");
            return -1;
        }

        // test range
        int mint = 100, maxt = 300, i;

        for(i = 0; i  < 1000; i++)
        {
            int r3 = random_int(mint, maxt);
            float r4 = random_float((float)mint, (float)maxt);

            // check that the number created in range
            if(r3 < mint || r3 > maxt || r4 < mint || r4 > maxt)
            {
                message(ERROR, "test of the random_int or random_float function failed");
                return -1;
            }
        }
    }

    // test random permutation
    {
        // create random permutation
        uint32_t *p = random_permutation(10);

        // count the number of each number
        int i, count[10] = {0};
        for(i = 0; i < 10; i++)
        {
            if(p[i] >= 0 && p[i] < 10)
            {
                count[p[i]]++;
            }
        }

        // free the array
        free(p);

        // check that the all numbers are in the array
        for(i = 0; i < 10; i++)
        {
            if(count[i] != 1)
            {
                message(ERROR, "test of the random_permutation function failed");
                return -1;
            }
        }
    }

    // test random sample
    {
        float input[9] = {0.1, 0.2, 0.3, 1, 2, 3, 10, 20, 30};
        float output[3] = {0};

        return_t result = random_sample(input, 9, 3, sizeof(float), output);
        if(result != SUCCESS)
        {
            message(ERROR, "test of the random_sample function failed");
            return -1;
        }

        // now test that the function sampled data correctly
        uint32_t s, i;
        for(s = 0; s < 3; s++)
        {
            int isFound = 0;
            for(i = 0; i < 9; i++)
            {
                if(output[s] == input[i])
                {
                    isFound = 1;
                    break;
                }
            }

            // check that the sample is correctly taken from the input set
            if(isFound == 0)
            {
                message(ERROR, "test of the random_sample function failed");
                return -1;
            }
        }
    }

    message(SUCCESS, "all operations are done successfully!");
    return 0;
}
