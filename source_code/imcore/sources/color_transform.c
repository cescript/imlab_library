#include <stdlib.h>
#include "private/imcore_private.h"

#undef RGB

struct color_t RGB(uint8_t r, uint8_t g, uint8_t b) {
    // create a color member
    struct color_t colorCode;
    // fill it with the given values
    colorCode.red   = r;
    colorCode.green = g;
    colorCode.blue  = b;
    // return the color
    return colorCode;
}

// get h,s,v component and create one 32 bit RGB integer from them
struct color_t HSV(uint8_t h, uint8_t s, uint8_t v) {
    // create an inline switch case for the hsv2rgb conversion
    int swich = (h / 43); // switch to this value
    int fpart = (h - 43*swich) * 6;

    uint8_t p = clamp((v * (255 - s)) >> 8, 0, 255);
    uint8_t q = clamp((v * (255 - ((s * fpart) >> 8))) >> 8, 0, 255);
    uint8_t t = clamp((v * (255 - ((s * (255 - fpart)) >> 8))) >> 8, 0, 255);

    // create a color and fill it with the corresponding BGR value
    struct color_t colorCode = RGB(0,0,0);

    if(swich == 0)      { colorCode = RGB(v,t,p); }
    else if(swich == 1) { colorCode = RGB(q,v,p); }
    else if(swich == 2) { colorCode = RGB(p,v,t); }
    else if(swich == 3) { colorCode = RGB(p,q,v); }
    else if(swich == 4) { colorCode = RGB(t,p,v); }
    else                { colorCode = RGB(v,p,q); }

    return colorCode;
}

// Integral Image Computation. This does not check any error so check everything before calling this.
#define compute_histogram(input_type, min_value, bin_size, matrix_ptr, hist_ptr, width, height, channels) \
    do                                                                                                    \
    {                                                                                                     \
        uint32_t _m, _n, _c, _idx, _step, _bin;                                                           \
        input_type *_matrix_ptr = matrix_ptr;                                                             \
        _step = channels * width;                                                                         \
        /* compute thehistogram */                                                                        \
        for (_n = 0; _n < height; _n++)                                                                   \
        {                                                                                                 \
            _idx = _n * _step;                                                                            \
            for (_m = 0; _m < width; _m++)                                                                \
            {                                                                                             \
                _bin = 0;                                                                                 \
                for (_c = 0; _c < channels; _c++)                                                         \
                {                                                                                         \
                    hist_ptr[_matrix_ptr[_idx + _c] - min_value + _bin]++;                                \
                }                                                                                         \
                _idx += channels;                                                                         \
                _bin += bin_size;                                                                         \
            }                                                                                             \
        }                                                                                                 \
        /* done */                                                                                        \
    } while (0);

return_t histogram(matrix_t *in, int32_t min, int32_t max, uint32_t **histogram)
{
    int cond = is_integer(in);
    check_condition(cond, ERROR_TYPE_MISMATCH, "input must be an integer matrix");

    // compute the histogram size
    uint32_t histogram_bin_size = (max - min + 1);
    uint32_t histogram_size = channels(in) * histogram_bin_size;

    // allocate the histogram if necessary
    if (histogram[0] == NULL)
    {
        histogram[0] = (uint32_t *)calloc(histogram_size, sizeof(uint32_t));
    }
    else
    {
        memset(histogram[0], 0, histogram_size * sizeof(uint32_t));
    }
    
    if(is_8s(in))
    {
        compute_histogram(int8_t, min, histogram_bin_size, mdata(in, 0), histogram[0], cols(in), rows(in), channels(in));
    }
    else if(is_8u(in))
    {
        compute_histogram(uint8_t, min, histogram_bin_size, mdata(in, 0), histogram[0], cols(in), rows(in), channels(in));
    }
    else if (is_16s(in))
    {
        compute_histogram(int16_t, min, histogram_bin_size, mdata(in, 0), histogram[0], cols(in), rows(in), channels(in));
    }
    else if (is_16u(in))
    {
        compute_histogram(uint16_t, min, histogram_bin_size, mdata(in, 0), histogram[0], cols(in), rows(in), channels(in));
    }
    else if (is_32s(in))
    {
        compute_histogram(int32_t, min, histogram_bin_size, mdata(in, 0), histogram[0], cols(in), rows(in), channels(in));
    }
    else if (is_32u(in))
    {
        compute_histogram(uint32_t, min, histogram_bin_size, mdata(in, 0), histogram[0], cols(in), rows(in), channels(in));
    }

    return SUCCESS;
}

// otsu algorithm for threshold value detection
static uint32_t otsu(uint32_t *hist, uint32_t hist_length) {

    uint32_t i, t, thr = 0;
    uint32_t sum  = 0, sum1  = 0, sum2  = 0;
    uint32_t wsum = 0, wsum1 = 0, wsum2 = 0;
    float avg1, avg2, sigma = 0;

    for( i=0; i < hist_length; i++) {
        wsum += i*hist[i];
        sum   += hist[i];
    }

    for(t=0; t < hist_length; t++) {

        sum1 += hist[t];
        if( sum1 == 0 ) { continue; }

        sum2 = (sum-sum1);
        if( sum2 == 0) { break; }

        wsum1 += t*hist[t];
        wsum2 = wsum - wsum1;

        avg1 = (float) wsum1 / sum1;
        avg2 = (float) wsum2 / sum2;

        float std = sum1*sum2*(avg1-avg2)*(avg1-avg2);

        if(std > sigma) { sigma = std; thr = t; }
     }
    return thr;
}

uint32_t imotsu(matrix_t *in) {

    int cond = is_image(in) & (channels(in) == 1);
    check_condition(cond, -1, "input must be a grayscale image");
    // get histogram of the input image
    uint32_t *hist = (uint32_t*) calloc(256, sizeof(int));
    // get histogram for gray or color image
    uint32_t n;

    // TODO: create in_data pointer based on the input type
    uint8_t *in_data  = data(uint8_t, in);

    for(n=0; n < width(in)*height(in); n++) {
        hist[in_data[n]]++;
    }
    // compute otsu threshold on the input
    int otsuthr = otsu(hist, 256);
    free(hist);

    return otsuthr;
}

// use adaptive threshold to binarize the input image
return_t imbinarize(matrix_t *in, uint32_t block_width, uint32_t block_height, float threshold, matrix_t *out) {
    // allocate out before use it
    matrix_resize(out, height(in), width(in), 1);

    int cond1 = channels(in) == 1 && channels(out) == 1;
    check_condition(cond1, ERROR_DIMENSION_MISMATCH, "input and output must be grayscale image");

    int cond2 = is_image(in) & is_image(out);
    check_condition(cond2, ERROR_NOT_IMAGE, "input and output must be image");

    // fast memory intensive threshold
    uint32_t i, j, h = 0;

    // TODO: create in_data pointer based on the input type
    uint8_t *in_data  = data(uint8_t, in);
    uint8_t *out_data = data(uint8_t, out);

    matrix_t *sums = matrix_create(float);
    matrix_t *ssum = matrix_create(float);
    // compute the integral image
    integral(in, sums, ssum);

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
    // compute the block area
    float block_area = block_width*block_height;
    uint32_t sx, sy, ex, ey;

    //#pragma omp parallel for
    for(j = 0; j < height(in); j++) {
        for(i = 0; i < width(in); i++) {

            if(i > half_block_width && i < width(in)-half_block_width) {
                sx = i - half_block_width;
                ex = i + half_block_width;
            }
            else if(i < half_block_width) {
                sx = 0;
                ex = block_width - 1;
            }
            else {
                sx = width(in) - block_width - 1;
                ex = width(in) -1;
            }

            if(j > half_block_height && j < height(in)-half_block_height) {
                sy = j - half_block_height;
                ey = j + half_block_height;
            }
            else if(i < half_block_width) {
                sy = 0;
                ey = block_height - 1;
            }
            else {
                sy = height(in) - block_height - 1;
                ey = height(in) -1;
            }

            float sum = 255 * integral_get_float(sums, sx, sy, ex, ey, 0) / block_area;
            out_data[h] = in_data[h] > (sum - threshold) ? (uint8_t )255 : (uint8_t )0;
            h++;
        }
    }
    //done
    return SUCCESS;
}

return_t imthreshold(matrix_t *in, uint32_t threshold, matrix_t *out) {
    // allocate out before use it
    matrix_resize(out, height(in), width(in), 1);

    int cond1 = channels(in)==1 && channels(out)==1;
    check_condition(cond1, ERROR_DIMENSION_MISMATCH, "input and output must be grayscale image");

    int cond2 = is_image(in) & is_image(out);
    check_condition(cond2, ERROR_NOT_IMAGE, "input and output must be image");

    // fast memory intensive threshold
    uint32_t h;

    // TODO: create in_data pointer based on the input type
    uint8_t *in_data  = data(uint8_t, in);
    uint8_t *out_data = data(uint8_t, out);

    char hist[256] = {0};
    for(h=threshold; h < 256; h++) { hist[h] = 255; }
    // If dim==1, it is already grayscale
    //#pragma omp parallel for
    for(h=0; h < width(in)*height(in); h++) {
        out_data[h] = hist[ in_data[h] ];
    }
    //done
    return SUCCESS;
}

return_t rgb2gray(matrix_t *in, matrix_t *out) {
    // allocate out before use it
    matrix_resize(out, height(in), width(in), 1);

    int cond1 = channels(in)==3 && channels(out)==1;
    check_condition(cond1, ERROR_DIMENSION_MISMATCH, "input must be color and the output must be grayscale image");

    int cond2 = is_image(in) & is_image(out);
    check_condition(cond2, ERROR_NOT_IMAGE, "input and output must be image");

    // TODO: create in_data pointer based on the input type
    uint8_t *in_data  = data(uint8_t, in);
    uint8_t *out_data = data(uint8_t, out);

    uint32_t h;
    //#pragma omp parallel for
    for(h=0; h < width(in)*height(in); h++) {
        out_data[h] = clamp( (int)(28*in_data[3*h] + 153*in_data[3*h+1] + 74*in_data[3*h+2]) >> 8 , 0,255);
    }
    //done
    return SUCCESS;
}

return_t gray2rgb(matrix_t *in, matrix_t *out)
{
    // allocate out before use it
    matrix_resize(out, height(in), width(in), 3);

    int cond1 = channels(in) == 1 && channels(out) == 3;
    check_condition(cond1, ERROR_DIMENSION_MISMATCH, "input must be grayscale and the output must be color image");

    int cond2 = is_image(in) & is_image(out);
    check_condition(cond2, ERROR_NOT_IMAGE, "input and output must be image");

    // TODO: create in_data pointer based on the input type
    uint8_t *in_data = data(uint8_t, in);
    uint8_t *out_data = data(uint8_t, out);

    uint32_t h;
    //#pragma omp parallel for
    for (h = 0; h < width(in) * height(in); h++)
    {
        out_data[3 * h + 0] = in_data[h];
        out_data[3 * h + 1] = in_data[h];
        out_data[3 * h + 2] = in_data[h];
    }
    //done
    return SUCCESS;
}

// Multi-Channel Transforms
return_t rgb2hsv(matrix_t *in, matrix_t *out) {

    // allocate out before use it
    matrix_resize(out, height(in), width(in), 3);

    int cond1 = channels(in)==3 && channels(out)==3;
    check_condition(cond1, ERROR_DIMENSION_MISMATCH, "input and output must be color image");

    int cond2 = is_image(in) & is_image(out);
    check_condition(cond2, ERROR_NOT_IMAGE, "input and output must be image");

    uint32_t h = 0;
    int mx, mn;

    // TODO: create in_data pointer based on the input type
    uint8_t *in_data  = data(uint8_t, in);
    uint8_t *out_data = data(uint8_t, out);

    #pragma omp parallel for private(h, mx, mn)
    for(h=0; h < width(in)*height(in); h++) {

        mx = max(in_data[3*h+0], max(in_data[3*h+1], in_data[3*h+2]));
        mn = min(in_data[3*h+0], min(in_data[3*h+1], in_data[3*h+2]));
        // set value to it
        out_data[3*h + 0] = mx; out_data[3*h + 1] = 0; out_data[3*h + 2] = 0;
        if(mx == mn) { continue; }
        // set saturation
        out_data[3*h + 1] = 255*(mx-mn)/mx;

        if(mx == in_data[3*h+2]) { out_data[3*h+2] = clamp(     43*(in_data[3*h+1]-in_data[3*h+0])/(mx-mn), 0, 255 ); }
        if(mx == in_data[3*h+1]) { out_data[3*h+2] = clamp(85 + 43*(in_data[3*h+0]-in_data[3*h+2])/(mx-mn), 0, 255 ); }
        if(mx == in_data[3*h+0]) { out_data[3*h+2] = clamp(171+ 43*(in_data[3*h+2]-in_data[3*h+1])/(mx-mn), 0, 255 ); }
    }
    return SUCCESS;
}

return_t hsv2rgb(matrix_t *in, matrix_t *out) {

    // allocate out before use it
    matrix_resize(out, height(in), width(in), 3);

    int cond1 = channels(in)==3 && channels(out)==3;
    check_condition(cond1, ERROR_DIMENSION_MISMATCH, "input and output must be color image");

    int cond2 = is_image(in) & is_image(out);
    check_condition(cond2, ERROR_NOT_IMAGE, "input and output must be image");

    uint32_t h, swich, fpart;

    // TODO: create in_data pointer based on the input type
    uint8_t *in_data  = data(uint8_t, in);
    uint8_t *out_data = data(uint8_t, out);


    #pragma omp parallel for private(h, swich, fpart)
    for(h=0; h < width(in)*height(in); h++) {

        int v = in_data[3*h + 0];
        int s = in_data[3*h + 1];
        //int h = in_data[3*h + 2];
        // check if it is a grayscale color
        if( s == 0 ) { out_data[3*h + 0] = v; out_data[3*h + 1] = v; out_data[3*h + 2] = v;
            continue;
        }
        swich = (in_data[3*h + 2] / 43); // switch to this value
        fpart = (in_data[3*h + 2] - 43*swich) * 6;

        int p = (v * (255 - s)) >> 8;
        int q = (v * (255 - ((s * fpart) >> 8))) >> 8;
        int t = (v * (255 - ((s * (255 - fpart)) >> 8))) >> 8;

        if(swich == 0)      { out_data[3*h + 0] = p; out_data[3*h + 1] = t; out_data[3*h + 2] = v; }
        else if(swich == 1) { out_data[3*h + 0] = p; out_data[3*h + 1] = v; out_data[3*h + 2] = q; }
        else if(swich == 2) { out_data[3*h + 0] = t; out_data[3*h + 1] = v; out_data[3*h + 2] = p; }
        else if(swich == 3) { out_data[3*h + 0] = v; out_data[3*h + 1] = q; out_data[3*h + 2] = p; }
        else if(swich == 4) { out_data[3*h + 0] = v; out_data[3*h + 1] = p; out_data[3*h + 2] = t; }
        else                { out_data[3*h + 0] = q; out_data[3*h + 1] = p; out_data[3*h + 2] = v; }
    }
    return SUCCESS;
}

// General Multi Channel Color transform
return_t rgb2any(matrix_t *in, const float tr[3][4], matrix_t *out) {

    // allocate out before use it
    matrix_resize(out, height(in), width(in), 3);

    int cond1 = channels(in)==3 && channels(out)==3;
    check_condition(cond1, ERROR_DIMENSION_MISMATCH, "input and output must be color image");

    int cond2 = is_image(in) & is_image(out);
    check_condition(cond2, ERROR_NOT_IMAGE, "input and output must be image");

    // TODO: create in_data pointer based on the input type
    uint8_t *in_data  = data(uint8_t, in);
    uint8_t *out_data = data(uint8_t, out);

    int h = 0;

    float tr00[256]; //
    float tr01[256]; //
    float tr02[256]; //

    float tr10[256]; //
    float tr11[256]; //
    float tr12[256]; //

    float tr20[256]; //
    float tr21[256]; //
    float tr22[256]; //

    for(h=0; h < 256; h++) {
        tr00[h] = h*tr[0][0];
        tr01[h] = h*tr[0][1];
        tr02[h] = h*tr[0][2];
        tr10[h] = h*tr[1][0];
        tr11[h] = h*tr[1][1];
        tr12[h] = h*tr[1][2];
        tr20[h] = h*tr[2][0];
        tr21[h] = h*tr[2][1];
        tr22[h] = h*tr[2][2];
    }
    #pragma omp parallel for
    for(int h=0; h < width(in)*height(in); h++) {

        // get the input data to local variables,
        // this will allow input and output pointers to indicate the same position
        uint8_t d1 = in_data[3*h + 0];
        uint8_t d2 = in_data[3*h + 1];
        uint8_t d3 = in_data[3*h + 2];

        out_data[3*h + 2] = clamp(tr02[d1]+tr01[d2]+tr00[d3] + tr[0][3], 0, 255);
        out_data[3*h + 1] = clamp(tr12[d1]+tr11[d2]+tr10[d3] + tr[1][3], 0, 255);
        out_data[3*h + 0] = clamp(tr22[d1]+tr21[d2]+tr20[d3] + tr[2][3], 0, 255);
    }
    return SUCCESS;
}
// convert RGB color into Lab color space
void rgb2lab(struct color_t C1, float *L, float *a, float *b)
{
    float R = C1.red   < 10 ? C1.red  /3294.6f : powf((C1.red  /255.0f + 0.055f)/1.055f, 2.4f);
    float G = C1.green < 10 ? C1.green/3294.6f : powf((C1.green/255.0f + 0.055f)/1.055f, 2.4f);
    float B = C1.blue  < 10 ? C1.blue /3294.6f : powf((C1.blue /255.0f + 0.055f)/1.055f, 2.4f);

    // Observer. = 2°, Illuminant = D65
    float X = R * 0.4124f + G * 0.3576f + B * 0.1804f;
    float Y = R * 0.2126f + G * 0.7152f + B * 0.0722f;
    float Z = R * 0.0193f + G * 0.1192f + B * 0.9505f;

    X /= 0.9504f;
    Y /= 1.0000f;
    Z /= 1.0890f;

    X = X > 0.008856f ? powf(X, 0.333334f ) : 7.878f*X + 16.0f / 116.0f;
    Y = Y > 0.008856f ? powf(Y, 0.333334f ) : 7.878f*Y + 16.0f / 116.0f;
    Z = Z > 0.008856f ? powf(Z, 0.333334f ) : 7.878f*Z + 16.0f / 116.0f;

    // set the Lab values
    L[0] = ( 116.0f * Y ) - 16.0f;
    a[0] = 500.0f * ( X - Y );
    b[0] = 200.0f * ( Y - Z );
}

return_t image_set(matrix_t *in, uint32_t y, uint32_t x, struct color_t clr) {
    // check that the given point is inside the image
    if(x >= width(in) || y >= height(in)) {
        return ERROR_DIMENSION_MISMATCH;
    }
    if(channels(in) == 1) {
        uint8_t gray = RGB2GRAY(clr);
        matrix_set(in, y,x,0, &gray);
    } else {
        matrix_set(in, y,x,2, &clr.red);
        matrix_set(in, y,x,1, &clr.green);
        matrix_set(in, y,x,0, &clr.blue);
    }
    return SUCCESS;
}




return_t image_get(matrix_t *in, uint32_t y, uint32_t x, struct color_t *clr) {
    // check that the given point is inside the image
    if(x >= width(in) || y >= height(in)) {
        return ERROR_DIMENSION_MISMATCH;
    }
    if(channels(in) == 1) {
        uint8_t colorG;
        matrix_get(in, y,x,0, &colorG);
        *clr = RGB(colorG, colorG, colorG);
    } else {
        matrix_get(in, y,x,0, &clr->red);
        matrix_get(in, y,x,1, &clr->green);
        matrix_get(in, y,x,2, &clr->blue);
    }
    return SUCCESS;
}




// #define integral_get(in, x0, y0, x1, y1) _Generic( (in) )
