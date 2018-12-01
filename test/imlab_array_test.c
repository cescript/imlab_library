// test all functionalities of the library
#include <stdio.h>
#include <stdint.h>
#include "core.h"

int main() {

#define testType uint16_t

    uint32_t i,j,k,l;

    uint32_t DIM1 = 11;
    uint32_t DIM2 = 22;
    uint32_t DIM3 = 33;
    uint32_t DIM4 = 44;

    // ONE DIM TEST
    testType *oneDim = array_create(testType, DIM1);
    if(oneDim == NULL) {
        printf("cannot allocate memory for one dimensional array!\n");
    }
    else {
        for(i = 0; i < DIM1; i++) {
            oneDim[i] = (testType) (i / (double)DIM1);
        }
        // test it filled correctly
        uint32_t dim1sum = 0;
        for(i = 0; i < array_size(oneDim, 0); i++) {
            if(oneDim[i] == (testType)(i / (double)DIM1)) {
                dim1sum++;
            }
        }
        // free the array
        array_free(oneDim);
        if(dim1sum == DIM1) {
            printf("One dimensional array is verified!\n");
        }
        else {
            printf("One dimensional array cannot verified!\n");
        }
    }

    // USE TWO DIM ARRAY TO TEST
    testType **twoDim = array_create(testType, DIM1, DIM2);
    if(twoDim == NULL) {
        printf("cannot allocate memory for two dimensional array!\n");
    }
    else {

        for(i = 0; i < array_size(twoDim, 0); i++) {
            for(j = 0; j < array_size(twoDim, 1); j++) {
                twoDim[i][j] = (testType) (i / (double)(DIM1*DIM2));
            }
        }
        // test it filled correctly
        uint32_t dim2sum = 0;
        for(i = 0; i < DIM1; i++) {
            for(j = 0; j < DIM2; j++) {
                if(twoDim[i][j] == (testType) (i / (double)(DIM1*DIM2))) {
                    dim2sum++;
                }
            }
        }
        // free the array
        array_free(twoDim);
        if(dim2sum == DIM1*DIM2) {
            printf("Two dimensional array is verified!\n");
        }
        else {
            printf("Two dimensional array cannot verified!\n");
        }
    }

    // USE THREE DIM ARRAY TO TEST
    testType ***threeDim = array_create(testType, DIM1, DIM2, DIM3);
    if(threeDim == NULL) {
        printf("cannot allocate memory for three dimensional array!\n");
    }
    else {

        for(i = 0; i < array_size(threeDim, 0); i++) {
            for(j = 0; j < array_size(threeDim, 1); j++) {
                for(k = 0; k < array_size(threeDim, 2); k++) {
                    threeDim[i][j][k] = (testType) (i / (double) (DIM1 * DIM2 * DIM3));
                }
            }
        }
        // test it filled correctly
        uint32_t dim3sum = 0;
        for(i = 0; i < DIM1; i++) {
            for(j = 0; j < DIM2; j++) {
                for(k = 0; k < DIM3; k++) {
                    if (threeDim[i][j][k] == (testType) (i / (double) (DIM1 * DIM2 * DIM3))) {
                        dim3sum++;
                    }
                }
            }
        }
        // free the array
        array_free(threeDim);
        if(dim3sum == DIM1*DIM2*DIM3) {
            printf("Three dimensional array is verified!\n");
        }
        else {
            printf("Three dimensional array cannot verified!\n");
        }
    }

    // USE FOUR DIM ARRAY TO TEST
    testType ****fourDim = array_create(testType, DIM1, DIM2, DIM3, DIM4);
    if(fourDim == NULL) {
        printf("cannot allocate memory for four dimensional array!\n");
    }
    else {

        for(i = 0; i < array_size(fourDim, 0); i++) {
            for(j = 0; j < array_size(fourDim, 1); j++) {
                for(k = 0; k < array_size(fourDim, 2); k++) {
                    for(l = 0; l < array_size(fourDim, 3); l++) {
                        fourDim[i][j][k][l] = (testType) (i / (double) (DIM1 * DIM2 * DIM3 * DIM4));
                    }
                }
            }
        }
        // test it filled correctly
        uint32_t dim4sum = 0;
        for(i = 0; i < DIM1; i++) {
            for(j = 0; j < DIM2; j++) {
                for(k = 0; k < DIM3; k++) {
                    for(l = 0; l < DIM4; l++) {
                        if (fourDim[i][j][k][l] == (testType) (i / (double) (DIM1 * DIM2 * DIM3 * DIM4))) {
                            dim4sum++;
                        }
                    }
                }
            }
        }
        // free the array
        array_free(fourDim);
        if(dim4sum == DIM1*DIM2*DIM3*DIM4) {
            printf("Four dimensional array is verified!\n");
        }
        else {
            printf("Four dimensional array cannot verified!\n");
        }
    }

    printf("IMLAB array test complete\n");
    return 0;
}
