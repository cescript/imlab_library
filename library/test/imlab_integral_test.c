// test integral image method of the library
#include <stdio.h>
#include <stdint.h>
#include "core.h"
#include "imcore.h"

int main()
{
    message(SUCCESS, "starting to test IMLAB integral method");

    // generate sample data
    float data[] = {1,2,3,4,5,6,7,8,9};
    float sumsExpected[] = {1,3,6,5,12,21,12,27,45};
    float ssumExpected[] = {1,5,14,17,46,91,66,159,285};

    // create inputs
    matrix_t *input = matrix_create(float, 3,3,1, data);

    // compute the integral image
    matrix_t *sums = matrix_create(float, 3,3,1);
    matrix_t *ssum = matrix_create(float, 3,3,1);

    // compute the integral image
    integral(input, sums, ssum);

    // check results
    uint32_t r, c, idx = 0;
    for(r = 0; r < rows(input); r++)
    {
        for(c = 0; c < cols(input); c++)
        {
            if(atf(sums, r,c,0) != sumsExpected[idx] || atf(ssum, r,c,0) != ssumExpected[idx])
            {
                message(ERROR, "integral image computation failed!");
                printf("expected: %4.1f,%4.1f --- calculated: %4.1f,%4.1f\n", sumsExpected[idx], ssumExpected[idx], atf(sums, r,c,0), atf(ssum, r,c,0));
                return -1;
            }

            idx++;
        }
    }

    // free memory
    matrix_free(&input);
    matrix_free(&sums);
    matrix_free(&ssum);

    message(SUCCESS, "all operations are done successfully!");

    return 0;
}
