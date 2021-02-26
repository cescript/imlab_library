// test all functionalities of the library
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "core.h"
#include "iocore.h"

int main() {

    message(SUCCESS, "starting to test IMLAB matrix library");

    // create an output directory
    imlab_mkdir("results");

    // create matrix with a single argument
    matrix_t *matc_1 = matrix_create(uint8_t);
    matrix_t *matf_1 = matrix_create(float);
    // create a matrix with two arguments
    matrix_t *matc_2 = matrix_create(matc_1, mdata(matc_1, 0)); // make a clone of the matc_1
    matrix_t *matd_2 = matrix_create(matc_1, NULL); // just copy the size of matc_1
    // create a matrix with three arguments
    matrix_t *mati_3 = matrix_create(int, 100, 100); // create a 100x100 matrix
    matrix_t *matf_3 = matrix_create(float, 121, 121); // create a 121x121 matrix
    // create a matrix with four arguments
    matrix_t *matd_4 = matrix_create(double, 121, 121, 2); // create a 121x121x2 matrix
    // create a matrix with five arguments
    float matD[100] = {1.0f, 1.1f, 1.2f, 1.3f};
    matrix_t *matf_5 = matrix_create(float, 10, 10, 1, matD); // create a volume(100) matrix and initialize the data with matD[100]
    matrix_t *matd_5 = matrix_create(double, 121, 121, 2, NULL); // create a 121x121x2 matrix same as matd_4

    matrix_t *null_mat = matrix_null();
    matrix_view(null_mat);

    vector_t *null_vec = vector_null();
    vector_view(null_vec);

    // try to load/save the
    matrix_write(matf_5, "results//test_save.imat");
    matrix_t *matf_4 = matrix_read("results//test_save.imat");

    vector_t *vecf_1 = vector_create(float);
    // create a matrix with two arguments
    vector_t *vecc_2 = vector_create(uint8_t, 0); // create a vector which has initially 1 element capacity (not zero despite the argument) same as vecc_1
    vector_t *vecd_2 = vector_create(double, 1000); // create a vector which has initially 1000 element capacity
    // create a matrix with three arguments
    uint16_t vecD[6] = {1,2,3,4,5, 1024};
    vector_t *vecc_3 = vector_create(uint8_t, 100, NULL); // create a vector which has initially 100 element capacity
    vector_t *veci_3 = vector_create(uint16_t, 6, vecD); // create a vector which has initially 1024 element capacity and copy the vecD data in it

    printf("vector size: %d / %d\n", length(veci_3), capacity(veci_3));

    vector_push(veci_3, &vecD[0]);
    vector_push(veci_3, &vecD[1]);
    vector_push(veci_3, &vecD[2]);
    vector_push(veci_3, &vecD[3]);
    vector_push(veci_3, &vecD[4]);

    uint16_t val = 12;
    vector_fill(veci_3, &val);

    printf("vec3_i[%04d]: [", length(veci_3));
    for(uint32_t i = 0; i < length(veci_3); i++) {
        uint16_t elemI1;
        vector_get(veci_3, i, &elemI1);
        printf("%d ", elemI1);
    }
    printf("]\n");

    // try to load/save the
    vector_write(veci_3, "results//test_save.ivec");
    vector_free(&veci_3);
    veci_3 = vector_read("results//test_save.ivec");

    printf("vec3_i[%04d]: [", length(veci_3));
    for(uint32_t i = 0; i < length(veci_3); i++) {
        uint16_t elemI2 = at(uint16_t, veci_3, i);
        printf("%d ", elemI2);
    }
    printf("]\n");

    message(SUCCESS, "all operations are done successfully!");


    return 0;
}
