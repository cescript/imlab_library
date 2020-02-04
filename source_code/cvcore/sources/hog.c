#include <stdlib.h>
#include <math.h>
#include "private/iocore_private.h"
#include "private/cvcore_private.h"

uint32_t hog_parameters(int32_t width, uint32_t height, uint32_t channels, char options[], struct hog_parameters_t *model) 
{
    // fill the default values
    model->nbins     = 9;
    model->stride[0] = 2;
    model->stride[1] = 2;
    model->b_size[0] = 4;
    model->b_size[1] = 4;
    model->c_size[0] = 8;
    model->c_size[1] = 8;

    // get the values from the user input
    getopt(uint32_t, options, "bins", &model->nbins);
    getopt(uint32_t, options, "stride", model->stride);
    getopt(uint32_t, options, "block", model->b_size);
    getopt(uint32_t, options, "cell", model->c_size);

    // check the width and height is ok
    if( (width%model->c_size[1]) > 0 || (height%model->c_size[0]) > 0 ) 
    {
        message(ERROR, "image size should be divisible by the cell size");
        return 0;
    } 
    else 
    {
        // set the hog width and height
        model->hog_width  = (width) / model->c_size[1];
        model->hog_height = (height) / model->c_size[0];
        model->width  = model->hog_width *model->c_size[1];
        model->height = model->hog_height*model->c_size[0];
        model->block_width  = 1+(model->hog_width -model->b_size[1])/model->stride[1];
        model->block_height = 1+(model->hog_height-model->b_size[0])/model->stride[0];
        model->feature_size = model->b_size[0]*model->b_size[1]*model->nbins*model->block_width*model->block_height;

        // allocate memory fro the gradients and histograms
        model->cell_hist = (float*) calloc(model->hog_height*model->hog_width*model->nbins, sizeof(float));

        if(model->cell_hist == NULL) 
        {
            return 0;
        }
        model->grad_mag  = (float*) malloc(width*height*sizeof(float));
        if(model->grad_mag == NULL) 
        {
            free(model->cell_hist);
            return 0;
        }
        model->grad_ori  = (float*) malloc(width*height*sizeof(float));
        if(model->grad_ori == NULL) 
        {
            free(model->cell_hist);
            free(model->grad_ori);
            return 0;
        }
    }
    return model->feature_size;
}

// creates a HOG model
struct feature_t *hog_create(uint32_t width, uint32_t height, uint32_t channels, char options[]) 
{
    // create a feature model for the output
    struct feature_t *model = (struct feature_t*) malloc(sizeof(struct feature_t));
    check_memory(model, NULL);

    // fill the algorithm name
    model->algorithm = CV_HOG;
    model->image_width = width;
    model->image_height = height;
    model->image_channels = channels;

    // get space for the parameters of the algorithm
    model->parameters = malloc(sizeof(struct hog_parameters_t));
    check_memory(model->parameters) 
    {
        free(model);
        return NULL;
    }
    // fill the parameters
    model->feature_size = hog_parameters(width, height, channels, options, model->parameters);

    if(model->feature_size == 0) 
    {
        free(model->parameters);
        free(model);
        return NULL;
    }
    // get the default feature extraction method
    model->method = hog_extract;

    // return the created model
    return model;
}


void hog_view(struct feature_t *par_model) 
{
    // get the HOG parameters from the given generic model
    struct hog_parameters_t *model = par_model->parameters;

    printf("Parameters of the Histogram of Oriented Gradient Model\n");
    printf("Options:\n");
    printf("> Orientation     : %d\n", model->nbins);
    printf("> Block Size      : [%d %d]\n", model->b_size[0], model->b_size[1]);
    printf("> Cell Size       : [%d %d]\n", model->c_size[0], model->c_size[1]);
    printf("> Stride          : [%d %d]\n", model->stride[0], model->stride[1]);

    printf("Computed Values:\n");
    printf("> HOG window size : [%d %d]\n", model->hog_width, model->hog_height);
    printf("> HOG image size  : [%d %d]\n", model->width, model->height);
    printf("> BLOCK Size      : [%d %d]\n", model->block_width, model->block_height);
    printf("> Feature Size    : %d\n", model->feature_size);
}

uint32_t  hog_feature_size(struct feature_t *model) 
{
    return ((struct hog_parameters_t*)(model->parameters))->feature_size;
}

static void imgradient(matrix_t *in, float *mag, float *ori, int nbin) 
{
    uint32_t r, c;
    // compute the gradient in the inside of the image
    for(r = 1; r < rows(in) - 1; r++) 
    {
        for(c = 1; c < cols(in) - 1; c++) 
        {
            // get the x and y gradients
            float gx = (atui8(in, r, c + 1, 0) - atui8(in, r, c - 1, 0)) / 255.0f;
            float gy = (atui8(in, r + 1, c, 0) - atui8(in, r - 1, c, 0)) / 255.0f;

            // fill the gradient and magnitude arrays
            mag[c + r * cols(in)] = sqrt(gx * gx + gy * gy);
            ori[c + r * cols(in)] = map(fabs(atan2f(gy, gx)), 0.0f, 3.141593f, 0, nbin - 1);
        }
    }
    //done
}


static void hog_cell_histogram(float *mag, float *ori, float *cell_hist, int width, int height, int wi, int hj, struct hog_parameters_t *model) 
{
    uint32_t i,j, idx;

    // set pointers to the (wi,hj) position
    ori += wi + width * hj;
    mag += wi + width * hj;

    // loop in the window and compute the histogram for each cell
    for(j = 0; j < model->height; j++) 
    {
        uint32_t cj = j / model->c_size[0]; //*inc_j;
        for(i = 0; i < model->width; i++) 
        {
            // cell index of the current column
            uint32_t ci = i / model->c_size[1]; //*inc_i;

            uint32_t idx = model->nbins * (ci + model->hog_width * cj);

            //scale the ori in [0,1] and multiply by the nbins-1
            float fbin = ori[i];
            uint32_t ibin = (uint32_t)fbin;

            // computes hog for a cell ci,cj
            // use bilinear interpolation and cyclic index
            cell_hist[idx + ibin]   += (1-(fbin-ibin)) * mag[i];
            if(ibin == model->nbins-1) { ibin = -1; }
            cell_hist[idx + ibin + 1] += (fbin-ibin) * mag[i];
        }

        ori += width;
        mag += width;
    }
    //done
}

static void hog_block_histogram(float *cell_hist, float *feature, struct hog_parameters_t *model) 
{

    uint32_t i, bi,bj, ci,cj;
    float *hist = feature;

    for(cj=0; cj < model->hog_height-model->b_size[1]+1; cj+=model->stride[1]) 
    {
        for(ci=0; ci < model->hog_width-model->b_size[0]+1; ci+=model->stride[0]) 
        {

            int fcount = 0;
            // concatenate the histograms to create block histogram
            // compute the normalization constants
            float norm = imlab_epsilon;
            for(bj=0; bj < model->b_size[1]; bj++) 
            {
                for(bi=0; bi < model->b_size[0]; bi++) 
                {
                    for(i=0; i < model->nbins; i++) 
                    {
                        hist[fcount] = cell_hist[model->nbins*(ci+bi+model->hog_width*(cj+bj)) + i];
                        norm += hist[fcount];
                        fcount++;
                    }
                }
            }
            // normalize the histogram and fill the feature vector
            norm = 1/(norm);
            for(i=0; i < model->b_size[0]*model->b_size[1]*model->nbins; i++) 
            {
                hist[i] = sqrtf(hist[i]*norm);
            }
            hist += model->b_size[0]*model->b_size[1]*model->nbins;
        }
    }
    //done
}

// extract hog from the given image
return_t hog_extract(matrix_t *in, struct feature_t *par_model, float *feature) 
{
    // check_numeric(element, ERROR_TYPE_MISMATCH);

    int cond1 = is_image(in);
    check_condition(cond1, ERROR_TYPE_MISMATCH, "input must be uint8 array");

    int cond2 = channels(in) == 1;
    check_condition(cond2, ERROR_DIMENSION_MISMATCH, "input must be 2D arrays");

    // get the HOG parameters from the given model
    struct hog_parameters_t *model = par_model->parameters;

    // clear the magnitude, orientation and cell histograms vectors
    memset(model->grad_mag, 0, model->width * model->height * sizeof(float));
    memset(model->grad_ori, 0, model->width * model->height * sizeof(float));
    memset(model->cell_hist, 0, model->hog_height * model->hog_width * model->nbins * sizeof(float));

    ///step 1: get the gradient of the image
    imgradient(in, model->grad_mag, model->grad_ori, model->nbins);

    ///step 2: get nbin-gradient histogram of the input for each cell
    // clear cell_hist before call
    hog_cell_histogram(model->grad_mag, model->grad_ori, model->cell_hist, width(in), height(in), 0, 0, model);

    ///step 3: use cell features and merge those by b_size x b_size
    hog_block_histogram(model->cell_hist, feature, model);

    //done
    return SUCCESS;
}

// convert input feature map to HOG visualization
matrix_t *hog2image(float *feature, struct feature_t *par_model)
{
    // get the HOG parameters
    struct hog_parameters_t *model = par_model->parameters;

    // double the size of the cell for better visualization
    uint32_t cell_width  = 2 * model->c_size[0] + 1;
    uint32_t cell_height = 2 * model->c_size[1] + 1;

    // create an output image
    matrix_t *image = matrix_create(uint8_t, model->block_height * cell_height, model->block_width * cell_width, 3);

    // check that the necessary memories are allocated
    check_null(image, matrix_null());

    // find the number of histogram in one block
    uint32_t number_of_histograms_in_block = model->nbins * model->b_size[0] * model->b_size[1];

    // compute the orientation bins
    float orientation_step = 3.1415945f / model->nbins;
    
    // allocate space for cell normalization
    float *cell_histogram = (float *)calloc(model->nbins, sizeof(float));

    uint32_t b, ci, cj;
    for (cj = 0; cj < model->block_height; cj++)
    {
        for (ci = 0; ci < model->block_width; ci++)
        {
            // set the starting orientation
            float orientation = orientation_step / 2;

            // first find the normalized cell histogram
            float cell_sum = 0;
            for(b = 0; b < model->nbins; b++)
            {
                cell_histogram[b] = feature[number_of_histograms_in_block * (ci + cj * model->block_width) + b];
                cell_sum += cell_histogram[b];
            }

            // if the sum of the cell histogram is too low, do not print anything
            if (cell_sum < 1e-5)
            {
                continue;
            }

            // draw line for each orientation for the current cell
            for (b = 0; b < model->nbins; b++)
            {
                // draw the line with the appropriate orientation
                float x0 = model->c_size[0] * cos(orientation);
                float y0 = model->c_size[1] * sin(orientation);

                // assign pixels
                uint32_t x = ci * cell_width + model->c_size[0];
                uint32_t y = cj * cell_height + model->c_size[1];

                struct point_t p1 = point(x + x0, y + y0, 0);
                struct point_t p2 = point(x - x0, y - y0, 0);

                // get the corresponding value for the patch
                float fval = cell_histogram[b] / cell_sum;

                // map the value between 0-1
                uint32_t value = map(fval, 0, 1.0, 0, 255);

                // draw the line on the image
                draw_line(image, p1, p2, HSV(value, 200, value), 1);

                // get the nex orientation angle
                orientation += orientation_step;
            }
        }
    }

    // return the result
    return image;
}
