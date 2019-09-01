#include <stdlib.h>
#include <math.h>
#include "private/iocore_private.h"
#include "private/cvcore_private.h"

uint32_t hog_parameters(int32_t width, uint32_t height, char options[], struct hog_parameters_t *model) {

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
    if( (width%model->c_size[1]) > 0 || (height%model->c_size[0]) > 0 ) {
        message(ERROR, "image size should be divisible by the cell size");
        return 0;
    } else {
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
        if(model->cell_hist == NULL) {
            return 0;
        }
        model->grad_mag  = (float*) malloc(width*height*sizeof(float));
        if(model->grad_mag == NULL) {
            free(model->cell_hist);
            return 0;
        }
        model->grad_ori  = (float*) malloc(width*height*sizeof(float));
        if(model->grad_ori == NULL) {
            free(model->cell_hist);
            free(model->grad_ori);
            return 0;
        }
    }
    return model->feature_size;
}

// creates a HOG model
struct feature_t *hog_create(uint32_t width, uint32_t height, char options[]) {
    // create a feature model for the output
    struct feature_t *model = (struct feature_t*) malloc(sizeof(struct feature_t));
    check_memory(model, NULL);
    // fill the algorithm name
    model->algorithm = CV_HOG;
    model->image_width = width;
    model->image_height = height;
    // get space for the parameters of the algorithm
    model->parameters = malloc(sizeof(struct hog_parameters_t));
    check_memory(model->parameters) {
        free(model);
        return NULL;
    }
    // fill the parameters
    model->feature_size = hog_parameters(width, height, options, model->parameters);
    if(model->feature_size == 0) {
        free(model->parameters);
        free(model);
        return NULL;
    }
    // get the default feature extraction method
    model->method = hog_extract;
    // return the created model
    return model;
}


void hog_view(struct feature_t *par_model) {
    // get the HOG parameters from the given generic model
    struct hog_parameters_t *model = par_model->parameters;

    printf("Parameters of the Histogram of Oriented Gradient Model\n");
    printf("Options:\n");
    printf("> Orientation   : %d\n", model->nbins);
    printf("> Block Size    : [%d %d]\n", model->b_size[0], model->b_size[1]);
    printf("> Cell Size     : [%d %d]\n", model->c_size[0], model->c_size[1]);
    printf("> Stride        : [%d %d]\n", model->stride[0], model->stride[1]);

    printf("Computed Values:\n");
    printf("> HOG window size : [%d %d]\n", model->hog_width, model->hog_height);
    printf("> HOG image size  : [%d %d]\n", model->width, model->height);
    printf("> BLOCK Size      : [%d %d]\n", model->block_width, model->block_height);
    printf("> Feature Size    : %d\n", model->feature_size);
}

uint32_t  hog_feature_size(struct feature_t *model) {
    return ((struct hog_parameters_t*)(model->parameters))->feature_size;
}

static inline float get_gradx(matrix_t *in, uint32_t wx, uint32_t hy, uint32_t d) {
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

static inline float get_grady(matrix_t *in, uint32_t wx, uint32_t hy, uint32_t d) {
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

static void imgradient(matrix_t *in, float *mag, float *ori, int nbin) {

    uint32_t i,j,d;

    float grad,gx, gy, grad_x, grad_y;

    //@TODO add support for floating point images
    uint8_t *in_data = data(uint8_t, in);

    // fill the borders
    for(j=0; j < height(in); j++) {
        mag[0 + j*width(in)] = 0;
        mag[width(in)-1 + j*width(in)] = 0;
    }
    for(i=0; i < width(in); i++) {
        mag[i + 0*width(in)] = 0;
        mag[i + (height(in)-1)*width(in)] = 0;
    }
    // compute the gradient in the inside of the image
    for(j=1; j < height(in)-1; j++) {
        for(i=1; i < width(in)-1; i++) {
            grad = -2.0f;
            for(d=0; d < channels(in); d++) {
                // get the image gradient around i+wi, j+hj
                grad_x = (in_data[ channels(in)*(i+1 + j*width(in)) + d ] - in_data[ channels(in)*(i-1+  j*width(in)) + d ]) / 255.0f;
                grad_y = (in_data[ channels(in)*(i+(j+1)*width(in)) + d ] - in_data[ channels(in)*(i+(j-1)*width(in)) + d ]) / 255.0f;

                // get the gradient
                if(grad_x*grad_x + grad_y*grad_y > grad) {
                    grad   = grad_x*grad_x + grad_y*grad_y;
                    gx = grad_x;
                    gy = grad_y;
                }
            }
            mag[ i+j*width(in) ] = sqrt(grad);
            ori[ i+j*width(in) ] = (nbin-1)*(0.5 + atan2f(gx,gy)/(6.284)) ;//map( atan2f(gx,gy), -3.15, 3.15, 0, nbin-0.1 );;
        }
    }
    //done
}


static void hog_cell_histogram(float *mag, float *ori, float *cell_hist, int width, int height, int wi, int hj, struct hog_parameters_t *model) {

    uint32_t i,j,ci,cj, idx;

    float fbin;
    uint32_t ibin;

    // TODO: remove this
    if(width == height) {
    }
    // set pointers to the (wi,hj) position
    ori += wi+width*hj;
    mag += wi+width*hj;
    // loop in the window and compute the histogram for each cell
    for(j=0; j < model->height; j++) {
        cj = j/model->c_size[0];//*inc_j;
        for(i=0; i < model->width; i++) {
            // cell index of the current column
            ci = i/model->c_size[1];//*inc_i;

            idx = model->nbins*(ci+model->hog_width*cj);
            //scale the ori in [0,1] and multiply by the nbins-1
            fbin = ori[i];
            ibin = (uint32_t)fbin;
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


/*
void hog_cell_int_histogram(float **hist_int, float *cell_hist, int width, int height, int wi, int hj, struct hog_t *model) {

    int i,j,ci,cj, idx;

    for(cj=0; cj < model->hog_height; cj++) {
        for(ci=0; ci < model->hog_width; ci++) {
            ///InvArea*(ssum[i+j*M] + ssum[i+WS+(j+WS)*M] - ssum[i+WS+j*M] - ssum[i+(j+WS)*M])
            for(i=0; i < model->nbins; i++) {
                cell_hist[i] += hist_int[i][wi + hj*width]+hist_int[i][wi+ci*model->c_size[1] + (hj+cj*model->c_size[0])*width];
                cell_hist[i] -= hist_int[i][wi+ci*model->c_size[1] + hj*width]+hist_int[i][wi + (hj+cj*model->c_size[0])*width];
            }
            cell_hist += model->nbins;
        }
        cell_hist += model->nbins*(model->hog_width);
    }
    //done
}
*/
static void hog_block_histogram(float *cell_hist, float *feature, struct hog_parameters_t *model) {

    uint32_t i, bi,bj, ci,cj;
    float *hist = feature;

    for(cj=0; cj < model->hog_height-model->b_size[1]+1; cj+=model->stride[1]) {
        for(ci=0; ci < model->hog_width-model->b_size[0]+1; ci+=model->stride[0]) {

            int fcount = 0;
            // concatenate the histograms to create block histogram
            // compute the normalization constants
            float norm = imlab_epsilon;
            for(bj=0; bj < model->b_size[1]; bj++) {
                for(bi=0; bi < model->b_size[0]; bi++) {
                    for(i=0; i < model->nbins; i++) {
                        hist[fcount] = cell_hist[model->nbins*(ci+bi+model->hog_width*(cj+bj)) + i];
                        norm += hist[fcount];
                        fcount++;
                    }
                }
            }
            // normalize the histogram and fill the feature vector
            norm = 1/(norm);
            for(i=0; i < model->b_size[0]*model->b_size[1]*model->nbins; i++) {
                hist[i] = sqrtf(hist[i]*norm);
            }
            hist += model->b_size[0]*model->b_size[1]*model->nbins;
        }
    }
    //done
}

// extract hog from the given image
return_t hog_extract(matrix_t *in, struct feature_t *par_model, float *feature) {
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
/*
matrix_t* hog_detect(matrix_t *in, struct hog_t *model, struct glm_t *net, float threshold) {

    int i,j;
    // detection options
    static int MaxDet  = 1024;
    int WS, WindowSize = 20;
    float ScaleFactor  = 1.2;
    int stride = 10;


    matrix_t *out = matrix_create(in, in->_data);
    // @TODO add support for floatimg point images
    uint8_t *out_data = data(uint8_t, out);

    for(j=0; j < height(out)-model->height; j++) {
        for(i=0; i < width(out)-model->width; i++) {
            out_data[ channels(out)*(i+model->width/2 + width(out)*(j+model->height/2)) + 0 ] *= 0.2;
            out_data[ channels(out)*(i+model->width/2 + width(out)*(j+model->height/2)) + 1 ] *= 0.2;
            out_data[ channels(out)*(i+model->width/2 + width(out)*(j+model->height/2)) + 2 ] *= 0.2;
        }
    }

    /// cell histogram will be kept here
    float *cell_hist = (float*) malloc(model->hog_height*model->hog_width*model->nbins*sizeof(float));

    ///step 1: get the gradient of the image
    float  *grad_mag  = (float*)  malloc(width(in)*height(in)*sizeof(float));
    float  *grad_ori  = (float*)  malloc(width(in)*height(in)*sizeof(float));

    imgradient(in, grad_mag, grad_ori, model->nbins);

    matrix_t *feature = matrix_create(float, 1,model->feature_size);
    float *feature_data = data(float, feature);

    float label;

    //#pragma omp parallel for private(i,j, label)
    for(j=0; j < height(in)-model->height; j+=stride) {
        //printf("Progress: %3.2f\r", 100.0f*j/(height(in)-model->height));
        for(i=0; i < width(in)-model->width; i+=stride) {
            //hog_extract(I,i,j,feature->data[0], model);

            ///step 2: get nbin-gradient histogram of the input for each cell
            memset(cell_hist, 0, model->hog_height*model->hog_width*model->nbins*sizeof(float));
            // clear cell_hist before call
            hog_cell_histogram(grad_mag, grad_ori, cell_hist, width(in), height(in), i, j, model);
            //hog_cell_int_histogram(hist_int, cell_hist, width(in), height(in), i,j, model);
            ///step 3: use cell features and merge those by b_size x b_size
            hog_block_histogram(cell_hist, feature_data, model);

            glm_predict(feature, &label, net);
            //label = max2(-1, min2(1, label));
            label = label > threshold ? 1:-1;
            //#pragma omp critical

            out_data[ channels(out)*(i+model->width/2 + width(out)*(j+model->height/2)) + 0 ] += 76*(label+1);
            out_data[ channels(out)*(i+model->width/2 + width(out)*(j+model->height/2)) + 1 ] += 96*(label+1);
            out_data[ channels(out)*(i+model->width/2 + width(out)*(j+model->height/2)) + 2 ] += 10*(label+1);

        }
    }
    free(cell_hist);
    free(grad_mag);
    free(grad_ori);

    return out;
}
*/

/*
matrix_t* hog_fast_detect(matrix_t *in, struct hog_t *model, GLM *net, float threshold) {

    int i,j,k, ii,jj;
    // detection options
    static int MaxDet  = 1024;
    int WS, WindowSize = 20;
    float ScaleFactor  = 1.2;

    matrix_t *out = imclone(in);

    for(j=0; j < height(out)-model->height; j++) {
        for(i=0; i < width(out)-model->width; i++) {
            out_data[ channels(out)*(i+model->width/2 + width(out)*(j+model->height/2)) + 0 ] *= 0.2;
            out_data[ channels(out)*(i+model->width/2 + width(out)*(j+model->height/2)) + 1 ] *= 0.2;
            out_data[ channels(out)*(i+model->width/2 + width(out)*(j+model->height/2)) + 2 ] *= 0.2;
        }
    }


    ///step 1: get the gradient of the image
    float  *grad_mag  = (float*)  malloc(width(in)*height(in)*sizeof(float));
    float  *grad_ori  = (float*)  malloc(width(in)*height(in)*sizeof(float));

    imgradient(in, grad_mag, grad_ori, model->nbins);

    int ci,cj, idx;

    int ImageCellinRow = height(in) / model->c_size[0];
    int ImageCellinCol = width(in)  / model->c_size[1];

    float fbin;
    int ibin;

    /// cell histogram will be kept here
    float *cell_hist = (float*) malloc(ImageCellinRow*ImageCellinCol*model->nbins*sizeof(float));

    // loop in the window and compute the histogram for each cell
    for(cj=0; cj < ImageCellinRow; cj++) {
        jj = cj*model->c_size[0];
        for(ci=0; ci < ImageCellinCol; ci++) {
            // cell index of the current column
            ii = ci*model->c_size[1];

            idx = model->nbins*(ci + ImageCellinCol*cj);
            // compute histogram for the ii,jj cell
            for(j=jj; j < jj+model->c_size[0]; j++) {
                for(i=ii; i < ii+model->c_size[1]; i++) {

                    fbin = grad_ori[ i + j*width(in) ];
                    ibin = (int)fbin;
                    // computes hog for a cell ci,cj
                    // use bilinear interpolation and cyclic index
                    cell_hist[idx + ibin]   += (1-(fbin-ibin))*grad_mag[i + j*width(in)];
                    if(ibin == model->nbins-1) { ibin = -1; }
                    cell_hist[idx + ibin + 1] += (fbin-ibin)*grad_mag[i + j*width(in)];

                }
            }
        }
    }

    float *feature;
    float label, norm;

    // create solver and find the transform function
    float (*tform)(float);
    int solv = net->options.solver;

    if(solv == LSRL1 || solv == LSRL2) { tform = lsr_transform; }
    if(solv == SVRL1 || solv == SVRL2) { tform = svr_transform; }
    if(solv == LREL1 || solv == LREL2) { tform = lre_transform; }
    if(solv == SVML1 || solv == SVML2) { tform = svm_transform; }

    //#pragma omp parallel for private(i,j, label)
    for(cj=0; cj < ImageCellinRow-model->b_size[0]; cj++) {
        //printf("Progress: %3.2f\r", 100.0f*cj/(ImageCellinRow-model->b_size[0]));
        for(ci=0; ci < ImageCellinCol-model->b_size[1]; ci++) {
            //hog_extract(I,i,j,feature->data[0], model);

            // create block histogram
            norm  = IM_EPS;
            feature = cell_hist + model->nbins*(ci + ImageCellinCol*cj);

            for(j=0; j < model->b_size[0]; j++) {
                for(k = 0; k < model->nbins * model->b_size[1]; k++ ) {
                    norm += feature[k];
                }
                feature += model->nbins*ImageCellinCol;
            }

            norm = 1.0f/norm;
            feature = cell_hist+ model->nbins*(ci + ImageCellinCol*cj);
            idx = 0;
            label = net->bias[0];
            for(j=0; j < model->b_size[0]; j++) {
                for(k = 0; k < model->nbins * model->b_size[1]; k++ ) {
                    label += net->beta[0][idx++]*sqrt(feature[k]*norm);
                }
                feature += model->nbins*ImageCellinCol;
            }

            label = tform(label);
            if(label > threshold) {
                printf("%3.2f\n", label);
            }
            //label = max2(-1, min2(1, label));
            label = 1;//label > threshold ? 1:-1;
            i = ci*model->c_size[1];
            j = cj*model->c_size[0];
            out_data[ channels(out)*(i+model->width/2 + width(out)*(j+model->height/2)) + 0 ] += 76*(label+1);
            out_data[ channels(out)*(i+model->width/2 + width(out)*(j+model->height/2)) + 1 ] += 96*(label+1);
            out_data[ channels(out)*(i+model->width/2 + width(out)*(j+model->height/2)) + 2 ] += 10*(label+1);
        }
    }
    free(cell_hist);
    free(grad_mag);
    free(grad_ori);

    return out;
}
*/
/*
// extract hog from the given image
float*** hog_extract(matrix_t *in, float *feature, struct hog_t *model) {

    int i,j, ci, cj;

    // strange way to keep 3d histogram
    float ***cell_norm = (float***) calloc(model->hog_height,sizeof(float**));
    for(i=0; i < model->hog_height; i++) {
        cell_norm[i] = (float**) calloc(model->hog_width,sizeof(float*));
        for(j=0; j < model->hog_width; j++) {
            cell_norm[i][j] = (float*) calloc(4*model->nbins,sizeof(float));
        }
    }

    float *hist = (float*) calloc(model->nbins,sizeof(float));
    ///step 1: get gradient of the input for each cell
    for(cj=0; cj < model->hog_height; cj++) {
        for(ci=0; ci < model->hog_width; ci++) {

            // computes hog for a cell ci,cj
            cell_histogram(data, ci, cj, width, height, model->c_size[1], model->c_size[0], model->nbins, hist);

            // compute energy of the histogram vector
            float l1 = IM_EPS, l2 = IM_EPS, l2h = IM_EPS;
            for(i=0; i < model->nbins; i++) {
                l1  += hist[i]; // l1 sum of the vector
                l2  += hist[i]*hist[i]; // l2 sum of the vector
                l2h += min2(hist[i], 0.2f)*min2(hist[i], 0.2f); // l2 hys
            }
            // compute the 4 normalized feature vector for each cell
            for(i=0; i < model->nbins; i++) {
                cell_norm[cj][ci][0*model->nbins  + i] = hist[i]/l1;
                cell_norm[cj][ci][1*model->nbins  + i] = sqrt(hist[i]/l1);
                cell_norm[cj][ci][2*model->nbins  + i] = hist[i]/sqrt(l2);
                cell_norm[cj][ci][3*model->nbins  + i] = max2(hist[i], 0.2f)/sqrt(l2h);

                // now we are done with the hist[i], clear it for the later pass
                hist[i] = 0;
            }
        }
    }
    return cell_norm;
    /// use cell features and merge those by b_size x b_size
}
*/

matrix_t *hog2image(float *feature, struct hog_parameters_t *model) {

    uint32_t i,j,b,ci,cj;

    uint32_t cell_width  = model->c_size[0] * 2;
    uint32_t cell_height = model->c_size[1] * 2;

    matrix_t *image = matrix_create(uint8_t, model->block_height*cell_height,model->block_width*cell_width, 1);
    matrix_t *patch = matrix_create(uint8_t, cell_height, cell_width, 1);
    matrix_t *rotpatch = matrix_create(uint8_t, cell_height, cell_width, 1);

    if(image == NULL || patch == NULL || rotpatch == NULL) {
        return NULL;
    };
    // @TODO add support for floatimg point images
    uint8_t *rotpatch_data = data(uint8_t, rotpatch);
    uint8_t *patch_data    = data(uint8_t, patch);
    uint8_t *image_data    = data(uint8_t, image);

    for(i=0; i < cell_width; i++) {
        for(j=-2; j < 3; j++) {
            patch_data[i + (height(patch)/2+j)*width(patch)] = 255;
        }
    }
    int NumHistInBlock = model->nbins*model->b_size[0]*model->b_size[1];

    float ori = 0;
    float ori_step = 180.0f/model->nbins;
    for(b=0; b < model->nbins; b++) {
        //create an image in b direction
        imtransform(patch, rot2tform(width(patch)/2, height(patch)/2, ori, 1), rotpatch);
        ori += ori_step;

        for(cj=0; cj < model->block_height; cj++) {
            for(ci=0; ci < model->block_width; ci++) {

                // assign pixels
                int idx_w = ci*cell_width;
                int idx_h = cj*cell_height;
                int idx;

                float sc = feature[ NumHistInBlock*(ci+cj*model->block_width) + b ];

                for(j=0; j < cell_height; j++) {
                    for(i=0; i < cell_width; i++) {
                        idx = idx_w+i+ (idx_h+j)*width(image);
                        // suppress if the values are similar
                        image_data[idx] += rotpatch_data[i + j*width(rotpatch)] * (sc*sc*10);
                    }
                }


            // continue with the other cells
            }
        }
    }
    matrix_free(&patch);
    matrix_free(&rotpatch);

    return image;
}
