// test all functionalities of the library
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "core.h"
#include "prcore.h"
#include "alcore.h"

// test the given algorithm and print the results
#define TEST_FUN(_values, _expected_result, _size, _type, _algorithm, ...)\
do{\
    _type *sorted_values = malloc(_size * sizeof(_type));\
    if(sorted_values == NULL)\
    {\
        printf("Sort test failed!\n");\
        return -1;\
    }\
    memcpy(sorted_values, _values, _size * sizeof(_type));\
    clock_t start = clock();\
    _algorithm(sorted_values, _size, sizeof(_type), __VA_ARGS__);\
    clock_t end = clock() - start;\
    unsigned int i;\
    for(i = 0; i < _size; i++)\
    {\
        if(sorted_values[i] != _expected_result[i])\
        {\
            break;\
        }\
    }\
    if(i != _size)\
    {\
        printf("Sort algorithm %s failed to sort data correctly!\n", #_algorithm);\
    }\
    else\
    {\
        printf("Time spend using %s: %5.3f milliseconds\n", #_algorithm, 1000 * ((float)end) / CLOCKS_PER_SEC);\
    }\
    free(sorted_values);\
}while(0);\

// create a comparison function which selects the largest double values in an array
int compare_doubles2(const void* _a, const void* _b) { return (*(double*)_a > *(double*)_b) ? 1 : (*(double*)_a < *(double*)_b) ? -1 : 0; }
int compare_doubles3(void *context, const void* _a, const void* _b) { return (*(double*)_a > *(double*)_b) ? 1 : (*(double*)_a < *(double*)_b) ? -1 : 0; }

int main(int argc, char *argv[])
{
    message(SUCCESS, "starting to test IMLAB sort methods");

    // decide the size of the test data
    unsigned int SortArraySize = 1000000;
    if(argc > 1)
    {
        SortArraySize = atoi(argv[1]);
    }
    printf("Sorting a double array with the size %d\n", SortArraySize);

    // set random seed
    random_seed();
    
    // create an array to hold the values
    double *values = malloc(SortArraySize * sizeof(double));

    // create an array to hold the sorted values and expected results
    double *result = malloc(SortArraySize * sizeof(double));
    
    // create a random permutation of indices
    uint32_t *indices = random_permutation(SortArraySize);

    // randomly insert the numbers into the array
    unsigned int i;
    for(i = 0; i < SortArraySize; i++)
    {
        result[i] = ((double)i) / 10.0;
        values[indices[i]] = result[i];
    }

    // free indices
    free(indices);

    // test sorting methods
    TEST_FUN(values, result, SortArraySize, double, quicksort, &compare_doubles3, NULL);

    // compare the results with platform specific function
    TEST_FUN(values, result, SortArraySize, double, qsort, &compare_doubles2);

    // free arrays and results
    free(values);
    free(result);
    
    message(SUCCESS, "all operations are done successfully!");

    return 0;
}