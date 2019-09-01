#include "private/core_private.h"
#include "private/iocore_private.h"

/*
    READ IMAGES IN BGR FORMAT AND STORE IN matrix_t CLASS

    I(i,j).RED   = I->DATA[ 2 + (3*i + WIDTH*j)
    I(i,j).GREEN = I->DATA[ 1 + (3*i + WIDTH*j)
    I(i,j).BLUE  = I->DATA[ 0 + (3*i + WIDTH*j)
*/

matrix_t *imread(char *filename) {

    uint8_t *out_data = NULL;
    uint32_t out_width = 0;
    uint32_t out_height = 0;
    uint32_t out_channels = 0;
    // read the bitmap from the file
    return_t ret_val = read_bmp(filename, &out_data, &out_width, &out_height, &out_channels);
    check_return(ret_val, matrix_null());

    // create the matrix and return it (the out_data pointer will be the pointer of the matrix)
    return matrix_create(uint8_t, out_height, out_width, out_channels, out_data, 1);
}

// IMAGE->data[ dim*(width + height*WIDTH) + bgr ]
return_t imload(char *filename, matrix_t *out) {
    // check that the given matrx is an image
    check_image(out, ERROR_NOT_IMAGE);
    // use the existing memory of the matrix to read bmp
    uint8_t *out_data = out->_data;
    uint32_t out_width = width(out);
    uint32_t out_height = height(out);
    uint32_t out_channels = channels(out);
    // read the bitmap from the file
    return_t ret_val = read_bmp(filename, &out_data, &out_width, &out_height, &out_channels);
    // check that everything done successfully
    check_return(ret_val, ret_val);
    // return success message
    return SUCCESS;
}

return_t imwrite(matrix_t *in, char *filename) {
    // check that the input is an image
    check_image(in, ERROR_NOT_IMAGE);
    // write the bitmap file into the given file
    return_t ret_val = write_bmp(filename, data(uint8_t, in), width(in), height(in), channels(in));
    // check that everything done successfully
    check_return(ret_val, ret_val);
    // return success message
    return SUCCESS;
}

#define cast_and_normalize(type1,type2, in, out, mini, maxi, length) \
do{\
    int i;\
    type1 *dataIn = in;\
    type2 *dataOut = out;\
    for(i=0; i < length; i++) {\
        dataOut[i] = 255*(dataIn[i]-mini)/(maxi-mini);\
    }\
}while(0);\

#define cast_and_clamp(type1,type2, in, out, mini, maxi, length) \
do{\
    int i;\
    type1 *dataIn = in;\
    type2 *dataOut = out;\
    for(i=0; i < length; i++) {\
        dataOut[i] = clamp(dataIn[i],mini, maxi);\
    }\
}while(0);\

#define cast_and_minimax(type, in, mini, maxi, minpos, maxpos, length) \
do{\
    int i;\
    type *dataIn = in;\
    for(i=0; i < length; i++) {\
        if (mini > dataIn[i]) { mini = dataIn[i]; }\
        else if (maxi < dataIn[i]) { maxi = dataIn[i]; }\
    }\
}while(0);\


return_t matrix2image(matrix_t *in, uint8_t opt, matrix_t *out) {

    // check that the input is an image
    check_condition(is_numeric(in), ERROR_TYPE_MISMATCH, "input must be a numeric array");
    check_condition(is_image(out), ERROR_TYPE_MISMATCH, "output must be uint8 type array");

    matrix_resize(out, rows(in), cols(in), channels(in));

    float mini = 1e32;
    float maxi = -mini;

    uint32_t i;


    // find min max by checking the matrix

    // TODO: create in_data pointer based on the input type
    float *in_data = data(float, in);
    uint8_t *out_data = data(uint8_t, out);

    if(opt == 0) {
        for(i=0; i < cols(in)*rows(in)*channels(in); i++) {
            if (mini > in_data[i]) { mini = in_data[i]; }
            else if (maxi < in_data[i]) { maxi = in_data[i]; }
        }
    }else if(opt == 1) {
        mini = 0;
        maxi = 1;
    } else {
        mini = 0;
        maxi = 255;
    }
    if(is_image(in)) {

    }




    //printf("minimaxi: %3.5f,%3.5f\n", mini, maxi);
    // normalize each channel between 0-1
    if(opt == 0 || opt == 1) {
        for(i=0; i < cols(in)*rows(in)*channels(in); i++) {
            out_data[i] = 255*(in_data[i]-mini)/(double)(maxi-mini);
        }
    } else {
        for(i=0; i < cols(in)*rows(in)*channels(in); i++) {
            out_data[i] = clamp(in_data[i], mini, maxi);
        }
    }

    return SUCCESS;
}

/*
matrix_tdata2image(float *data, float mini, float maxi, int width, int height, int channels) {

    int i;
    // data range is assumed to be in [0,1]
    matrix_t out = matrix_create(width,height,channels);

    for(i = 0; i < width*height*channels; i++) {
        data(out)[i] = 255*(data[i]-mini)/(maxi-mini);
    }
    return out;
}

float *image2data(matrix_t in, float scale) {

    int i;
    float *data = (float*) malloc(width(in)*height(in)*channels(in)*sizeof(float));
    // data range is assumed to be in [0,1]
    for(i = 0; i < width(in)*height(in)*channels(in); i++) {
         data[i] = (float)data(in)[i] * scale;
    }
    return data;
}
*/
