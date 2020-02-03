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

static inline float get_gradx(matrix_t *in, uint32_t wx, uint32_t hy, uint32_t d) 
{
    //@TODO add support for floating point images
    uint8_t *in_data = data(uint8_t, in);
    // gx = data[x+1][y] - data[x-1][y];
    if(wx > 0 && wx < width(in))
        return (in_data[ channels(in)*(wx+1+  hy*width(in)) + d ] - in_data[ channels(in)*(wx-1+  hy*width(in)) + d ]) / 255.0f;
    else return 0;
    // if x==0 --> gx = data[1][y] - 0
    //if(wx == 0)                 return (in_data[channels(in)*(hy*width(in) + 1) + d]) / 255.0f;
    // if x==w --> gx = 0 - data[w-1][y]
    //if(wx == width(in)-1)        return -(in_data[channels(in)*(hy*width(in) + width(in)-2) + d]) / 255.0f;
}

static inline float get_grady(matrix_t *in, uint32_t wx, uint32_t hy, uint32_t d) 
{
    //@TODO add support for floating point images
    uint8_t *in_data = data(uint8_t, in);
    // gy = data[x][y+1] - data[x][y-1];
    if(hy > 0 && hy < width(in)) return (in_data[ channels(in)*(wx+(hy+1)*width(in)) + d ] - in_data[ channels(in)*(wx+(hy-1)*width(in)) + d ]) / 255.0f;
    else return 0;
    // if y==0 --> gy = data[x][1] - 0
    //if(hy == 0)                 return (in_data[channels(in)*(width(in) + wx) + d]) / 255.0f;
    // if x==w --> gy = 0 - data[x][y-1]
    //if(hy == height(in)-1)        return -(in_data[channels(in)*((height(in)-2)*width(in) + wx) + d]) / 255.0f;
}

static void imgradient(matrix_t *in, float *mag, float *ori, int nbin) 
{
    uint32_t i,j,d;

    float grad,gx, gy, grad_x, grad_y;

    //@TODO add support for floating point images
    uint8_t *in_data = data(uint8_t, in);

    // fill the borders
    for(j=0; j < height(in); j++) 
    {
        mag[0 + j*width(in)] = 0;
        mag[width(in)-1 + j*width(in)] = 0;
    }
    for(i=0; i < width(in); i++) 
    {
        mag[i + 0*width(in)] = 0;
        mag[i + (height(in)-1)*width(in)] = 0;
    }
    // compute the gradient in the inside of the image
    for(j=1; j < height(in)-1; j++) 
    {
        for(i=1; i < width(in)-1; i++) 
        {
            grad = -2.0f;
            for(d=0; d < channels(in); d++) 
            {
                // get the image gradient around i+wi, j+hj
                grad_x = (in_data[ channels(in)*(i+1 + j*width(in)) + d ] - in_data[ channels(in)*(i-1+  j*width(in)) + d ]) / 255.0f;
                grad_y = (in_data[ channels(in)*(i+(j+1)*width(in)) + d ] - in_data[ channels(in)*(i+(j-1)*width(in)) + d ]) / 255.0f;

                // get the gradient
                if(grad_x*grad_x + grad_y*grad_y > grad) 
                {
                    grad   = grad_x*grad_x + grad_y*grad_y;
                    gx = grad_x;
                    gy = grad_y;
                }
            }
            mag[i + j * width(in)] = sqrt(grad);
            ori[i + j * width(in)] = map(fabs(atan2f(gy, gx)), 0.0f, 3.141593f, 0, nbin - 1);
        }
    }
    //done
}


static void hog_cell_histogram(float *mag, float *ori, float *cell_hist, int width, int height, int wi, int hj, struct hog_parameters_t *model) 
{
    uint32_t i,j,ci,cj, idx;

    // set pointers to the (wi,hj) position
    ori += wi+width*hj;
    mag += wi+width*hj;

    // loop in the window and compute the histogram for each cell
    for(j=0; j < model->height; j++) 
    {
        cj = j/model->c_size[0];//*inc_j;
        for(i=0; i < model->width; i++) 
        {
            // cell index of the current column
            ci = i/model->c_size[1];//*inc_i;

            idx = model->nbins*(ci+model->hog_width*cj);

            //scale the ori in [0,1] and multiply by the nbins-1
            float fbin = ori[i];
            uint32_t ibin = (uint32_t)fbin;

            // computes hog for a cell ci,cj
            // use bilinear interpolation and cyclic index
            cell_hist[idx + ibin]   += (1-(fbin-ibin))*mag[i];
            if(ibin == model->nbins-1) { ibin = -1; }
            cell_hist[idx + ibin + 1] += (fbin-ibin)*mag[i];
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
    // get the HOG parameters from the given model
    struct hog_parameters_t *model = par_model->parameters;
    // keep the histogram for each bin
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
