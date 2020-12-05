#include <stdlib.h>
#include "private/imcore_private.h"


// Integral Image Computation. This does not check any error so check everything before calling this.
#define compute_integral_image(input_type, output_type, max_value, imgep, sumsp, ssump, width, height, channels)      \
    do                                                                                                                \
    {                                                                                                                 \
        uint32_t _m, _n, _c, _idx, _step;                                                                             \
        output_type *_sm = malloc(channels * sizeof(output_type));                                                    \
        output_type *_ss = malloc(channels * sizeof(output_type));                                                    \
        output_type _inv1 = 1.0 / max_value;                                                                          \
        output_type _inv2 = _inv1 * _inv1;                                                                            \
        input_type *_imge = imgep;                                                                                    \
        output_type *_sums = sumsp;                                                                                   \
        output_type *_ssum = ssump;                                                                                   \
        _step = channels * width;                                                                                     \
        for (_c = 0; _c < channels; _c++)                                                                             \
        {                                                                                                             \
            (_sums)[_c] = (_imge)[_c] * _inv1;                                                                        \
            (_ssum)[_c] = (_imge)[_c] * (_imge)[_c] * _inv2;                                                          \
        }                                                                                                             \
        /* first compute the first row */                                                                             \
        for (_m = 1; _m < width; _m++)                                                                                \
        {                                                                                                             \
            _idx += channels;                                                                                         \
            for (_c = 0; _c < channels; _c++)                                                                         \
            {                                                                                                         \
                (_sums)[_idx + _c] = (_imge)[_idx + _c] * _inv1 + (_sums)[_idx + _c - channels];                      \
                (_ssum)[_idx + _c] = (_imge)[_idx + _c] * (_imge)[_idx + _c] * _inv2 + (_ssum)[_idx + _c - channels]; \
            }                                                                                                         \
        }                                                                                                             \
        /* compute the table completely */                                                                            \
        for (_n = 1; _n < height; _n++)                                                                               \
        {                                                                                                             \
            for (_c = 0; _c < channels; _c++)                                                                         \
            {                                                                                                         \
                _sm[_c] = 0, _ss[_c] = 0;                                                                             \
            }                                                                                                         \
            _idx = _n * _step;                                                                                        \
            for (_m = 0; _m < width; _m++)                                                                            \
            {                                                                                                         \
                for (_c = 0; _c < channels; _c++)                                                                     \
                {                                                                                                     \
                    _sm[_c] += (_imge)[_c + _idx] * _inv1;                                                            \
                    _ss[_c] += (_imge)[_c + _idx] * (_imge)[_c + _idx] * _inv2;                                       \
                    /* fill the arrays */                                                                             \
                    (_sums)[_c + _idx] = _sm[_c] + (_sums)[_c + _idx - _step];                                        \
                    (_ssum)[_c + _idx] = _ss[_c] + (_ssum)[_c + _idx - _step];                                        \
                }                                                                                                     \
                _idx += channels;                                                                                     \
            }                                                                                                         \
        }                                                                                                             \
        free(_sm);                                                                                                    \
        free(_ss);                                                                                                    \
        /* done */                                                                                                    \
    } while (0);

// General Multi Channel integral image transform
return_t integral(matrix_t *in, matrix_t *sums, matrix_t *ssum)
{
    int cond1 = is_image(in) || is_numeric(in);
    check_condition(cond1, ERROR_NOT_IMAGE, "input must be uint8 or floating point matrix");

    int cond2 = is_numeric(sums) && is_numeric(ssum) && is_sametype(sums, ssum);
    check_condition(cond2, ERROR_TYPE_MISMATCH, "outputs must be floating point matrix with a same type");

    // allocate out before use it
    matrix_resize(sums, height(in), width(in), channels(in));
    matrix_resize(ssum, height(in), width(in), channels(in));

    uint32_t in_width = width(in);
    uint32_t in_height = height(in);
    uint32_t in_channels = channels(in);

    // branch according to the input output types
    if (is_8u(in))
    {
        if (is_32f(sums))
        {
            compute_integral_image(uint8_t, float, 255.0, mdata(in, 0), mdata(sums, 0), mdata(ssum, 0), in_width, in_height, in_channels);
        }
        else
        {
            compute_integral_image(uint8_t, double, 255.0, mdata(in, 0), mdata(sums, 0), mdata(ssum, 0), in_width, in_height, in_channels);
        }
    }
    else if (is_32f(in))
    {
        if (is_32f(sums))
        {
            compute_integral_image(float, float, 1.0, mdata(in, 0), mdata(sums, 0), mdata(ssum, 0), in_width, in_height, in_channels);
        }
        else
        {
            compute_integral_image(float, double, 1.0, mdata(in, 0), mdata(sums, 0), mdata(ssum, 0), in_width, in_height, in_channels);
        }
    }

    return SUCCESS;
}
//TODO: FIX ME bug for all types we cannot access void pointer like this
float integral_get_float(matrix_t *in, int x0, int y0, int x1, int y1, int c)
{
    float summation = atf(in, y1, x1, c);

    // extract T(y1,x0-1)
    summation -= (x0 > 0) ? atf(in, y1, x0 - 1, c) : 0;
    // extract T(y0-1,x1)
    summation -= (y0 > 0) ? atf(in, y0 - 1, x1, c) : 0;
    // add T(y0-1,x0-1)
    summation += (x0 > 0 && y0 > 0) ? atf(in, y0 - 1, x0 - 1, c) : 0;

    // return the summation
    return summation;
}

double integral_get_double(matrix_t *in, int x0, int y0, int x1, int y1, int c)
{
    double summation = atd(in, y1, x1, c);

    // extract T(y1,x0-1)
    summation -= (x0 > 0) ? atd(in, y1, x0 - 1, c) : 0;
    // extract T(y0-1,x1)
    summation -= (y0 > 0) ? atd(in, y0 - 1, x1, c) : 0;
    // add T(y0-1,x0-1)
    summation += (x0 > 0 && y0 > 0) ? atd(in, y0 - 1, x0 - 1, c) : 0;

    // return the summation
    return summation;
}

return_t box_filter(matrix_t *in, uint32_t block_width, uint32_t block_height, matrix_t *out)
{
    // allocate out before use it
    matrix_resize(out, height(in), width(in), channels(in));

    int cond1 = is_image(in) & is_image(out);
    check_condition(cond1, ERROR_NOT_IMAGE, "input and output must be image");

    // loop variables
    uint32_t m = 0, n = 0, k = 0;

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

    // pre calculate the normalizer for the box
    double areaInv = 255.0 / (block_width * block_height);

    // compute the integral image
    matrix_t *integral1 = matrix_create(float, rows(in), cols(in), channels(in));
    matrix_t *integral2 = matrix_create(float, rows(in), cols(in), channels(in));

    integral(in, integral1, integral2);

    // run over the image and compute the box blur
    for (n = half_block_height; n < height(in) - half_block_height; n++)
    {
        for (m = half_block_width; m < width(in) - half_block_width; m++)
        {
            // filter the channels seperately
            for (k = 0; k < channels(in); k++)
            {
                atui8(out, n, m, k) = clamp(areaInv * integral_get_float(integral1, m - half_block_width, n - half_block_height, m + half_block_width, n + half_block_height, k), 0, 255);
            }
        }
    }

    matrix_free(&integral1);
    matrix_free(&integral2);

    return SUCCESS;

}