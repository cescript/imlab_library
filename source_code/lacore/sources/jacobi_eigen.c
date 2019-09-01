#include "private/lacore_private.h"
#include <math.h>

// find the maximum off diagonal element of the symmetric in matrix and return the position and value of the pivot
float get_pivot(matrix_t *in, uint32_t *iMax, uint32_t *jMax)
{
    uint32_t i, j;

    // initialize the pivot value
    float rValue = 0;
    float mValue = -1;

    // start from the first row and increment steps for each row by cols(in)
    uint32_t current_step = 0;

    for (i = 0; i < rows(in); i++)
    {
        // get the starting position of the current row
        float *in_row = mdata(in, current_step);

        for (j = i + 1; j < cols(in); j++)
        {
            if (fabs(in_row[j]) > mValue)
            {
                rValue = in_row[j];
                mValue = fabs(rValue);
                iMax[0] = i;
                jMax[0] = j;
            }
        }

        // skip to the next row
        current_step += cols(in);
    }

    // return the pivot value
    return rValue;
}

// apply Given rotation(i,j, theta) to the in matrix(k,l)
void apply_given_rotation_kl(matrix_t *in, uint32_t k, uint32_t l, uint32_t i, uint32_t j, float ctheta, float stheta)
{
    float *Skl = data(float, in, k, l);
    float *Sij = data(float, in, i, j);

    // rotate Ski and Akj
    float Spkl = ctheta * Skl[0] - stheta * Sij[0];
    float Spij = stheta * Skl[0] + ctheta * Sij[0];

    // S' = R * S
    Skl[0] = Spkl;
    Sij[0] = Spij;
}

// apply Given rotation(k,l, theta) to the in matrix, assume that the in is symmetric
void apply_given_rotation(matrix_t *evalues, matrix_t *evectors, uint32_t i, uint32_t j, float ctheta, float stheta)
{
    // do loop for the rest of the changed values
    uint32_t k;

    // for the first group, k < (i,j) so ({i,j},k) is the left triangles
    for (k = 0; k < i; k++)
    {
        // S'ki = c * Ski - s * Skj;
        // S'kj = s * Ski + c * Skj;
        apply_given_rotation_kl(evalues, k, i, k, j, ctheta, stheta);
    }
    // skip k = i and continue with the next group, k > i, so (k,i) and (j,k) are the left triangles
    for (k = i + 1; k < j; k++)
    {
        // S'ik = c * Sik - s * Skj;
        // S'kj = s * Sik + c * Skj;
        apply_given_rotation_kl(evalues, i, k, k, j, ctheta, stheta);
    }
    // skip k = j and continue with the next group k > (i,j) so (k,{i,j}) is the left triangles
    for (k = j + 1; k < cols(evalues); k++)
    {
        // S'ik = c * Sik - s * Sjk;
        // S'jk = s * Sik + c * Sjk;
        apply_given_rotation_kl(evalues, i, k, j, k, ctheta, stheta);
    }

    // rotate the eigen vectors
    if (evectors != matrix_null())
    {
        for (k = 0; k < cols(evectors); k++)
        {
            // E'ki = c * Eki - s * Ekj;
            // E'kj = s * Eki + c * Ekj;
            apply_given_rotation_kl(evectors, k, i, k, j, ctheta, stheta);
        }
    }   
}

return_t eig(matrix_t *input, matrix_t *evalues, matrix_t *evectors)
{
    int cond0 = channels(input) == 1 && is_numeric(input);
    check_condition(cond0, ERROR_DIMENSION_MISMATCH, "input must be single channel numeric matrix");

    int cond1 = rows(input) == cols(input);
    check_condition(cond1, ERROR_DIMENSION_MISMATCH, "rows and cols of the input must be equal");

    int cond2 = evalues != matrix_null() ? is_numeric(evalues) : 1 && evectors != matrix_null() ? is_numeric(evectors) : 1;
    check_condition(cond2, ERROR_TYPE_MISMATCH, "output must be a numeric matrix");

    // find the dimesnion and sweep count
    uint32_t N = rows(input);
    uint32_t sweepPoint = N * (N - 1) / 2;
    uint32_t maxNumberOfIter = 200 * sweepPoint;

    // get copy of the input matrix, if evalues given, work on the given matrix
    if (evalues != matrix_null())
    {
        // if evalues is empty, fill it with the copy of the input
        if (rows(evalues) != N && cols(evalues) != N)
        {
            matrix_copy(input, evalues);
        }
        // if evalues is allocated, asssume that it is already copied
        else
        {
            /* nothing to do */
        }
    }
    // if evalues is not given, work on the input matrix
    else
    {
        evalues = input;
    }

    if (evectors != matrix_null())
    {
        // if evectors is empty, fill it with the identity matrix
        if (rows(evectors) != N && cols(evectors) != N)
        {
            matrix_resize(evectors, N, N, 1);

            uint32_t d;

            float zero = 0;
            float one = 1;

            // fill all element with zeros
            matrix_fill(evectors, &zero);

            // set diagonals to ones
            for (d = 0; d < cols(evectors); d++)
            {
                matrix_set(evectors, d, d, 0, &one);
            }
        }
        // if evectors is allocated, asssume that it is already identity matrix
        else
        {
            /* nothing to do */
        }
    }

    uint32_t iter = 0;

    // apply Jacobi Eigen Value algorithm
    while (++iter < maxNumberOfIter)
    {
        uint32_t iMax = 0;
        uint32_t jMax = 0;

        // get the pivot value, since upper triangle is searched iMax < jMax
        float Sij = get_pivot(evalues, &iMax, &jMax);

        // no non-diagonal element left, terminate the iteration here
        if (equal(Sij, 0, 1e-10))
        {
            break;
        }
        else
        {
            // get the important elements
            float Sii = atf(evalues, iMax, iMax);
            float Sjj = atf(evalues, jMax, jMax);

            // find the rotation angle
            float theta = atan2f(2 * Sij, Sjj - Sii) / 2.0f;

            // find sin and cos of the theta
            float s = sinf(theta);
            float c = cosf(theta);

            // apply Givens rotation to the input matrix (handle the special rows and columns first)

            // compute the value of S'(i,i)
            atf(evalues, iMax, iMax) = c * c * Sii - 2 * s * c * Sij + s * s * Sjj;
            // compute the value of S'(j,j)
            atf(evalues, jMax, jMax) = s * s * Sii + 2 * s * c * Sij + c * c * Sjj;

            // compute the value of S'(i,j) = S'(j,i) (must be zero after rotation)
            atf(evalues, iMax, jMax) = (c * c - s * s) * Sij + s * c * (Sii - Sjj);
            atf(evalues, jMax, iMax) = (c * c - s * s) * Sij + s * c * (Sii - Sjj);

            // apply Givens rotation to the input matrix (handle the rest of the matrix)
            apply_given_rotation(evalues, evectors, iMax, jMax, c, s);
        }
    }
    // printf("eigen values are found after [%05d] iteration!\n", iter);
    if (iter == maxNumberOfIter)
    {
        message(WARNING, "eig function reached the max number of sweeps. Output might be incorrect!");
        return WARNING;
    }

    // everything look okay
    return SUCCESS;
}