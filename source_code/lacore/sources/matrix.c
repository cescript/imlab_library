#include "private/lacore_private.h"
#include <stdlib.h>

/*
// Not well defined functions. Make them clear and write this file again

#define cast_and_transpose(type, in, out)\
do{\
    int i, j, c;\
    int ChannelSize = rows(in)*cols(in);\
    for(c=0; c < channels(in); c++) {\
        type *in_data  = data(in);\
        type *out_data = data(out);\
        for (i=0; i < rows(in); i++) {\
            for (j=0; j < cols(in); j++) {\
                out_data[c*ChannelSize + i+cols(out)*j] = in_data[c*ChannelSize + j+cols(in)*i];\
            }\
        }\
    }\
}while(0);\


// compute the transpose of the two dimensional matrix
return_t matrix_transpose(matrix_t in, matrix_t *out) {

    int cond1 = is_numeric(in) && type(in) == type(*out);
    check_arguments(cond1, "input and must must have the same numeric type", ERROR_TYPE_MISMATCH);

    matrix_resize(out[0], cols(in), rows(in), channels(in));

    if( channels(in) != 1 ) {
        print_warning("%s: transpose of multichannel matrix is not defined!\n Transposing each channel seperately!\n", __func__);
    }
    uint32_t type = type(in);
    // cast the numeric type and call transpose function
    if(type == IMLAB_8U || type == IMLAB_8S) {
        cast_and_transpose(uint8_t, in, out[0]);
    }else if(type == IMLAB_16U || type == IMLAB_16S) {
        cast_and_transpose(uint16_t, in, out[0]);
    }else if(type == IMLAB_32U || type == IMLAB_32S) {
        cast_and_transpose(uint32_t, in, out[0]);
    }else if(type == IMLAB_32F) {
        cast_and_transpose(float, in, out[0]);
    }else if(type == IMLAB_64F) {
        cast_and_transpose(double, in, out[0]);
    }
    // done
	return SUCCESS;
}

//#pragma omp parallel for private(c,i,j)
#define cast_and_multiply(type, inAd, inBd, outd)\
do{\
    int i, j, c;\
    int ChannelSize = rows(out)*cols(out);\
    for(c=0; c < channels(out); c++) {\
        type *inA_data = inAd + c*ChannelSize;\
        type *inB_data = inBd + c*ChannelSize;\
        type *out_data = outd + c*ChannelSize;\
        for (i=0; i < rows(out); i++) {\
            for (j=0; j < cols(out); j++) {\
                fdot4(type, inA_data+i*cols(inA), inB_data+j*cols(inB), out_data[j+cols(out)*i], cols(inA));\
            }\
        }\
    }\
}while(0);\

return_t matrix_multiply(matrix_t inA, matrix_t inB, matrix_t *out) {

    int cond1 = type(inA) == type(inB) == type(*out) && is_numeric(inA);
    check_arguments(cond1, "input and out must have same numeric type for multiplacation!", ERROR_TYPE_MISMATCH);

    int cond2 = channels(inA) == channels(inB);
    check_arguments(cond2, "input channels do not match!", ERROR_DIMENSION_MISMATCH);

    int cond3 = cols(inA) == rows(inB);
    check_arguments(cond3, "input dimensions do not match!", ERROR_DIMENSION_MISMATCH);
    // resize before use it
    matrix_resize(out[0], rows(inA), cols(inB), channels(inA));

    if( channels(inA) != 1 ) {
        print_warning("%s: multichannel matrix multiplacation is not defined!\n multiplying each channel seperately!\n", __func__);
    }

    // do you want to copy the data or just create a matrix with the same size
    matrix_t inBt = matrix_create(inB, 0);
    matrix_transpose(inB, &inBt);

    uint32_t type = type(inA);
    // cast the numeric type and call multilier function
    if(type == IMLAB_8U || type == IMLAB_8S) {
        cast_and_multiply(uint8_t, data(inA), data(inBt), data(out[0]));
    }else if(type == IMLAB_16U || type == IMLAB_16S) {
        cast_and_multiply(uint16_t, data(inA), data(inBt), data(out[0]));
    }else if(type == IMLAB_32U || type == IMLAB_32S) {
        cast_and_multiply(uint32_t, data(inA), data(inBt), data(out[0]));
    }else if(type == IMLAB_32F) {
        cast_and_multiply(float, data(inA), data(inBt), data(out[0]));
    }else if(type == IMLAB_64F) {
        cast_and_multiply(double, data(inA), data(inBt), data(out[0]));
    }

    matrix_free(inBt);
    // done
    return SUCCESS;
}
*/

// If the current diagonal element is zero, find the next element
// TODO: add imlab epsilon istead of 0.000001 thing
#define cast_and_divide(_type, inA, out)                                                                 \
    do                                                                                                  \
    {                                                                                                   \
        int i, j;                                                                                       \
        int c, d, row, col, swap;                                                                       \
        _type *swap_buffer = (_type *)malloc(max(cols(inA), cols(out)) * sizeof(_type));                \
        _type *inA_data = mdata(inA, 0);                                                                \
        _type *out_data = mdata(out, 0);                                                                \
        for (d = 0; d < rows(inA); d++)                                                                 \
        {                                                                                               \
            col = d, row = d, swap = 0;                                                                 \
            while (equal(inA_data[d + row * cols(inA)], 0, 0.0000001))                                  \
            {                                                                                           \
                row++;                                                                                  \
                if (row == rows(inA))                                                                   \
                {                                                                                       \
                    printf("Cannot solve A\\B, A is not inversible!\n");                                \
                   break;                                                                               \
                }                                                                                       \
                swap = 1;                                                                               \
            }                                                                                           \
            if (swap)                                                                                   \
            {                                                                                           \
                memcpy(swap_buffer, &inA_data[d * cols(inA)], cols(inA) * sizeof(_type));                \
                memcpy(&inA_data[d * cols(inA)], &inA_data[row * cols(inA)], cols(inA) * sizeof(_type)); \
                memcpy(&inA_data[row * cols(inA)], swap_buffer, cols(inA) * sizeof(_type));              \
                memcpy(swap_buffer, &out_data[d * cols(out)], cols(out) * sizeof(_type));                \
                memcpy(&out_data[d * cols(out)], &out_data[row * cols(out)], cols(out) * sizeof(_type)); \
                memcpy(&out_data[row * cols(out)], swap_buffer, cols(out) * sizeof(_type));              \
            }                                                                                           \
            double eliminator = 0;                                                                      \
            double diag = 0;                                                                            \
            for (row = 0; row < rows(inA); row++)                                                       \
            {                                                                                           \
                if (row != d)                                                                           \
                {                                                                                       \
                    eliminator = -inA_data[d + row * cols(inA)] / inA_data[d + d * cols(inA)];          \
                    for (col = 0; col < cols(inA); col++)                                               \
                    {                                                                                   \
                        inA_data[col + row * cols(inA)] += eliminator * inA_data[col + d * cols(inA)];  \
                    }                                                                                   \
                    for (col = 0; col < cols(out); col++)                                               \
                    {                                                                                   \
                        out_data[col + row * cols(out)] += eliminator * out_data[col + d * cols(out)];  \
                    }                                                                                   \
                }                                                                                       \
                else                                                                                    \
                {                                                                                       \
                    diag = 1.0 / inA_data[d + d * cols(inA)];                                           \
                    for (col = 0; col < cols(inA); col++)                                               \
                    {                                                                                   \
                        inA_data[col + row * cols(inA)] *= diag;                                        \
                    }                                                                                   \
                    for (col = 0; col < cols(out); col++)                                               \
                    {                                                                                   \
                        out_data[col + row * cols(out)] *= diag;                                        \
                    }                                                                                   \
                }                                                                                       \
            }                                                                                           \
        }                                                                                               \
        free(swap_buffer);                                                                              \
    } while (0);


return_t matrix_divide(matrix_t *inA, matrix_t *inB, matrix_t *out) 
{
    int cond1 = is_sametype(inA,inB) && is_sametype(inB, out) && is_numeric(inA);
    check_condition(cond1, ERROR_TYPE_MISMATCH, "input and out must have same numeric type for division");

    int cond2 = channels(inA) == channels(inB);
    check_condition(cond2, ERROR_DIMENSION_MISMATCH , "input channels do not match!");

    int cond3 = cols(inA) == rows(inA);
    check_condition(cond3, ERROR_DIMENSION_MISMATCH , "first input must be a square matrix");

    int cond4 = cols(inA) == rows(inB);
    check_condition(cond4, ERROR_DIMENSION_MISMATCH , "input dimensions do not match");

    int cond5 = channels(inA) == 1;
    check_condition(cond4, ERROR_DIMENSION_MISMATCH, "multichannel matrix division is not defined");

    // resize before use it
    matrix_resize(out, rows(inB), cols(inB), 1);
    matrix_t *sinA = matrix_create(inA, mdata(inA, 0)); // create a copy of the input matrix

    // copy the inA content to the output
    memcpy(mdata(out, 0), mdata(inB, 0), rows(inB)*cols(inB)*elemsize(inA));

    // cast the numeric type and call multilier function
    if (is_8u(inA) || is_8s(inA))
    {
        cast_and_divide(uint8_t, sinA, out);
    }
    else if (is_16u(inA) || is_16s(inA))
    {
        cast_and_divide(uint16_t, sinA, out);
    }
    else if (is_32u(inA) || is_32s(inA))
    {
        cast_and_divide(uint32_t, sinA, out);
    }
    else if (is_32f(inA))
    {
        cast_and_divide(float, sinA, out);
    }
    else if (is_64f(inA))
    {
        cast_and_divide(double, sinA, out);
    }

    matrix_free(&sinA);

    return SUCCESS;
}

