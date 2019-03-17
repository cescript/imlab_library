#ifndef __IMLAB_LACORE_H__
#define __IMLAB_LACORE_H__

#include <stdint.h>
#include "core.h"



inline float fast_atan2f(float x, float y);

/**
 * Computes the eigenvalues and eigenvectors of the input real symmetric matrix using the Jacobian Iteration method.
 * @param input A square, Real and Symmetric input matrix in numeric type format
 * @param evalues Output matrix for the eigenvalues of the #input. 
 * If the size of the evalues is not equal to the size of the input evalues matrix is resized and values of the input are copied into the new resized matrix before the algorithm.
 * If the size of the evalues is equal to the size of the input, than the function assumes that input is already copied into the evalues.
 * If the evalues is equal to matrix_null, algorithm works on the #input and eigenvalues are returned inside the input.
 * @param evectors Output matrix for the eigenvectors of the #input. 
 * If the size of the evectors is not equal to the size of the input evectors matrix is resized and set to identity matrix before the algorithm.
 * If the size of the evectors is equal to the size of the input, than the function assumes that evectors already an identity matrix.
 * If the evectors is equal to matrix_null, algorithm does not compute the eigen vectors
 * @return Returns the status (error, warning or success) of the computation. 
*/
return_t eig(matrix_t *input, matrix_t *evalues, matrix_t *evectors);

#endif //IMLAB_LACORE_H
