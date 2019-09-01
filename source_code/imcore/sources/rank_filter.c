#include <stdlib.h>
#include "private/imcore_private.h"

/* Implementation of Median Filtering in Constant Time algorithm */

uint32_t find_rank_median(uint32_t *hist, uint32_t length, uint32_t rank) {

    uint32_t clr = 0;
    uint32_t sum = 0;
    // find the cumulative sum and the first clr that pass the t threshold
    while(sum < rank && clr < length) {
        sum += hist[clr++];
    }
    return clr - 1;
}

#define rank_filter2d_with_type(_type, _length) do {\
    uint32_t *block_histogram = array_create(uint32_t, _length);\
    check_memory(block_histogram);\
    uint32_t **column_histogram = array_create(uint32_t, width(in), _length);\
    check_memory(column_histogram);\
    for(i = 0; i < width(in); i++) {\
        memset(column_histogram[i], 0, _length * sizeof(uint32_t));\
        for(j = 0; j < block_height; j++) {\
        uint8_t clr = at(uint8_t, in, j, i);\
        column_histogram[i][clr]++;\
        }\
    }\
    for(j = half_block_height; j < height(in) - half_block_height; j++) {\
        uint32_t y0 = j - half_block_height;\
        uint32_t y1 = j + half_block_height;\
        memset(block_histogram, 0, _length * sizeof(uint32_t));\
        for(i = 0; i < block_width; i++) {\
            for(h = 0; h < _length; h++) {\
                block_histogram[h] += column_histogram[i][h];\
            }\
        }\
        uint8_t *input_data_y0 = data(uint8_t, in, y0, 0);\
        uint8_t *input_data_y1 = data(uint8_t, in, y1, 0);\
        uint8_t *output_data_j = data(uint8_t, out, j, 0);\
        for(i = half_block_width; i < width(in) - half_block_width; i++) {\
            uint32_t x0 = i - half_block_width;\
            uint32_t x1 = i + half_block_width;\
            _type clr = 0;\
            uint32_t sum = 0;\
            while(sum < rank && clr < _length) {\
                sum += block_histogram[clr++];\
            }\
            output_data_j[i] = clr - 1;\
            for(h = 0; h < _length; h++) {\
                block_histogram[h] -= column_histogram[x0][h];\
                block_histogram[h] += column_histogram[x1][h];\
            }\
            column_histogram[x0][input_data_y0[x0]]--;\
            column_histogram[x0][input_data_y1[x0]]++;\
        }\
    }\
    array_free(block_histogram);\
    array_free(column_histogram);\
}while(0);



return_t rank_filter2d(matrix_t *in, uint32_t block_width, uint32_t block_height, float rank_order, matrix_t *out) {

    // allocate out before use it
    matrix_resize(out, height(in), width(in), 1);

    int cond1 = channels(in) == 1 && channels(out) == 1;
    check_condition(cond1, ERROR_DIMENSION_MISMATCH, "input and output must be grayscale image");

    int cond2 = is_image(in) & is_image(out);
    check_condition(cond2, ERROR_NOT_IMAGE, "input and output must be image");

    // fast memory intensive threshold
    uint32_t i, j, h = 0;

    // if the block is even sized, make it odd
    if(block_width % 2 == 0) {
        block_width = block_width + 1;
    }
    if(block_height % 2 == 0) {
        block_height = block_height + 1;
    }

    // get the half block size
    uint32_t half_block_width  = (block_width - 1) / 2;
    uint32_t half_block_height = (block_height - 1) / 2;

    uint32_t rank = (uint32_t)(rank_order * (block_width * block_height));

    // find the rank filter
    if(is_8u(in)) {
        rank_filter2d_with_type(uint8_t, UINT8_MAX);
    }
    else if(is_16u(in)) {
        rank_filter2d_with_type(uint16_t, UINT16_MAX);
    }

    return SUCCESS;

}

// find the median filtering result of the input image
return_t medfilt2(matrix_t *input, uint32_t filter_width, uint32_t filter_height, matrix_t *output) {
    return rank_filter2d(input, filter_width, filter_height, 0.5, output);
}
