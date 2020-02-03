#include <stdlib.h> // for malloc
#include <math.h>
#include <string.h> // for memset
#include "private/iocore_private.h" // for getopt
#include "private/cvcore_private.h"

// count the 0->1 and 1->0 flips in the code
static int lbp_numflip(int code, int neighbors) {
    int numflip = 0;
    int current = code%2; // get the last bit
    for (int j=1; j <= neighbors; j++) {
        int parity = (code >> (j%neighbors)) % 2; // slide the input and get the last bit
        if (parity != current) { numflip++; }
        current = parity;
    }
    return numflip;
}

static uint32_t *lbp_lookup(int neighbors, int max_flip, uint32_t *hist_max) {

    int i;
    int LookUpSize = pow(2,neighbors);

    uint32_t *look_up = (uint32_t*) calloc( LookUpSize,sizeof(uint32_t) );
    if(look_up == NULL) {
        return NULL;
    }

    uint32_t uniform_id = 0;
    // create look up table, if it is noise, bin it to last
    for(i=0; i < LookUpSize; i++) {
        if( lbp_numflip(i, neighbors) >  max_flip) {
            look_up[i] = UINT32_MAX;
        } else {
            look_up[i] = uniform_id++;
        }
    }
    for(i=0; i < LookUpSize; i++) {
        if( look_up[i] == UINT32_MAX ) {
            look_up[i] = uniform_id;
        }
    }
    // this will be neighbors*(neighbors-1) + 3
    *hist_max = uniform_id+1;
    // done
    return look_up;
}

// creates a default LBP model
uint32_t lbp_parameters(uint32_t width, uint32_t height, uint32_t channels, char options[], struct lbp_parameters_t *model) 
{
    // fill the default values
    model->radius    = 1;
    model->neighbors = 8;
    model->uniform   = 1;
    model->b_size[0] = 4;
    model->b_size[1] = 4;
    model->win_width  = width;
    model->win_height = height;
    // get the user given parameters
    getopt(uint32_t, options, "radius", &model->radius);
    getopt(uint32_t, options, "neighbors", &model->neighbors);
    // uniform (8,2), default value tells not to use uniform lbp
    getopt(uint32_t, options, "uniform", &model->uniform);
    getopt(uint32_t, options, "block", model->b_size);

    // number of neighbors must be between 1-16
    if(model->neighbors > 16 || model->neighbors < 1) 
    {
        model->neighbors  = max_2(min_2(model->neighbors, 16), 1);
        message(WARNING, "number of neighbors must be between 1-16");
    }

    int cell_in_col = (width -2*model->radius) / model->b_size[1];
    int cell_in_row = (height-2*model->radius) / model->b_size[0];

    /// if uniform mode is on, compute the uniform struct lbp_t
    model->hist_bin_size = pow(2, model->neighbors);
    if(model->uniform != model->neighbors) 
    {
        /// create the uniform lbp look up table
        model->table = lbp_lookup(model->neighbors, model->uniform, &model->hist_bin_size);
        if(model->table == NULL) {
            message(ERROR, "not enough memory");
            return 0;
        }
    }
    model->feature_size = model->hist_bin_size*cell_in_col*cell_in_row;

    // return the created model
    return model->feature_size;
}

// "radius:1 neighbors:8 uniform:0 "
struct feature_t *lbp_create(uint32_t width, uint32_t height, uint32_t channels, char options[]) 
{
    // create a feature model for the output
    struct feature_t *model = (struct feature_t*) malloc(sizeof(struct feature_t));
    check_memory(model, NULL);

    // fill the algorithm name
    model->algorithm = CV_LBP;
    model->image_width = width;
    model->image_height = height;
    model->image_channels = channels;
    // get space for the parameters of the algorithm

    model->parameters = malloc(sizeof(struct lbp_parameters_t));
    if(model->parameters == NULL) 
    {
        free(model);
        return NULL;
    }
    // fill the parameters
    model->feature_size = lbp_parameters(width, height, channels, options, model->parameters);

    if(model->feature_size == 0) 
    {
        free(model->parameters);
        free(model);
        return NULL;
    }
    // get the default feature extraction method
    model->method = lbp_extract;

    // return the created model
    return model;
}

void lbp_view(struct feature_t *par_model) 
{
    struct lbp_parameters_t *model = par_model->parameters;

    printf("Parameters of the Local binary Pattern Model\n");
    printf("Options:\n");
    printf("> Radius          : %d\n", model->radius);
    printf("> Neighbors       : %d\n", model->neighbors);
    printf("> Block Size      : [%d %d]\n", model->b_size[0], model->b_size[1]);
    printf("> Uniform LBP     : %d\n", model->uniform);

    printf("Computed Values:\n");
    printf("> Feature Size    : %d\n", model->feature_size);
    printf("> Histogram Bin   : %d\n", model->hist_bin_size);
    printf("> LBP image size  : [%d %d]\n", model->win_width - 2*model->radius, model->win_height - 2*model->radius);
}

uint32_t  lbp_feature_size(struct feature_t *model) 
{
    if(model == NULL) 
    {
        return 0;
    }
    return ((struct lbp_parameters_t*)model)->feature_size;
}

uint32_t  lbp_width(struct lbp_parameters_t *model) 
{
    if(model == NULL) 
    {
        return 0;
    }
    return model->win_width - 2*model->radius;
}

uint32_t  lbp_height(struct lbp_parameters_t *model) 
{
    if(model == NULL) 
    {
        return 0;
    }
    return model->win_height - 2*model->radius;
}
/*
void Computestruct lbp_t8(const unsigned char *src, unsigned char *dst, int cols, int rows)
{
	int numberOfCols;
	int value;
	int center;
	int r0,r1,r5,r6,r7,r8;
	const unsigned char *p2,*p3,*p4;

	p2 = src;
	p3 = p2 + cols;
	p4 = p3 + cols;

	dst = dst + cols - 1;

	rows -= 2;
	cols -= 2;

	do {
		dst += 2;
		p2 += 2;
		r0 = *(p2 - 2);
		r1 = *(p2 - 1);
		p3 += 2;
		r7 = *(p3 - 2);
		r8 = *(p3 - 1);
		p4 += 2;
		r6 = *(p4 - 2);
		r5 = *(p4 - 1);
		numberOfCols = cols;
		do {
			center = r8;
			center += 2;
			value = (((center - r0) & 0x80000000) >> 31);
			r0 = r1;
			value |= (((center - r1) & 0x80000000) >> 30);
			r1 = *p2++;
			value |= (((center - r1) & 0x80000000) >> 29);
			value |= (((center - r7) & 0x80000000) >> 24);
			r7 = r8;
			r8 = *p3++;
			value |= (((center - r8) & 0x80000000) >> 28);
			value |= (((center - r6) & 0x80000000) >> 25);
			r6 = r5;
			value |= (((center - r5) & 0x80000000) >> 26);
			r5 = *p4++;
			value |= (((center - r5) & 0x80000000) >> 27);
			*dst++ = (unsigned char) value;
		} while (--numberOfCols);
	} while (--rows);
}
*/

void lbp_3x3(matrix_t *in, int *lbp_feat) 
{

    uint32_t i,j;
    unsigned char cx, code;
    // @TODO add support for floating point images
    uint8_t *in_data = data(uint8_t, in);
    // extract lbp on 3x3 neighborhood
	for(j=1; j < height(in)-1; j++) 
    {
		for(i=1; i < width(in)-1; i++)
        {

            cx = in_data[i + j*width(in)];
			code = 0;
			code |= (in_data[i-1 + (j  )*width(in)] > cx) << 7;
			code |= (in_data[i-1 + (j+1)*width(in)] > cx) << 6;
			code |= (in_data[i   + (j+1)*width(in)] > cx) << 5;
			code |= (in_data[i+1 + (j+1)*width(in)] > cx) << 4;
			code |= (in_data[i+1 + (j  )*width(in)] > cx) << 3;
			code |= (in_data[i+1 + (j-1)*width(in)] > cx) << 2;
			code |= (in_data[i   + (j-1)*width(in)] > cx) << 1;
			code |= (in_data[i-1 + (j-1)*width(in)] > cx) << 0;
			//lbp_feat[i-1 + (j-1)*(imge->width-2)] = code;
			*lbp_feat++ = code;
		}
	}
}

void lbp_nxn(matrix_t *in, int *lbp_feat, struct lbp_parameters_t *model) 
{

    uint32_t i,j;

    // extract lbp on NxN neighborhood
	for(i=0; i < model->neighbors; i++) 
    {
        float xp =  model->radius*cosf(2*imlab_pi*i / (float)model->neighbors);
        float yp = -model->radius*sinf(2*imlab_pi*i / (float)model->neighbors);

        // floor and ceil the subpixel to use bilinear interpolation
        int fx = (int) xp;
        int fy = (int) yp;

        int cx = (int) xp+1;
        int cy = (int) yp+1;

        //use opencv bilinear format f(x,y) = [1-x, x][ f(0,0),f(0,1); f(1,0),f(1,1)] [1-y ; y]
        //                           f(x,y) = f(0,0)(1-x)(1-y)+f(1,0)x(1-y)+f(0,1)(1-x)y+f(1,1)xy
        // fractional part
		float ty = xp - fy;
		float tx = yp - fx;
		// set interpolation weights
		float w1 = (1-tx) * (1-ty);
		float w2 =    tx  * (1-ty);
		float w3 = (1-tx) *    ty;
		float w4 =    tx  *    ty;
        // @TODO add support for floating point images
        uint8_t *in_data = data(uint8_t, in);
		float fxy;
        for(j=model->radius; j < height(in)-model->radius; j++) 
        {
            for(i=model->radius; i < width(in)-model->radius; i++) 
            {

                // neighbor is on the subpixel, use bilinear interpolation
                fxy  = w1*in_data[ i+fx + (j+fy)*width(in) ] + w2*in_data[ i+cx + (j+fy)*width(in) ];
                fxy += w3*in_data[ i+fx + (j+cy)*width(in) ] + w4*in_data[ i+cx + (j+cy)*width(in) ];

                lbp_feat[ i-model->radius + (j-model->radius)*(width(in)-2*model->radius) ] |= (fxy > in_data[i + j*width(in)]);
            }
        }
        printf("lbp_feat: %d\n",  lbp_feat[ i-model->radius + (j-model->radius)*(width(in)-2*model->radius) ] );

        // shift the computed value
        lbp_feat[ i-model->radius + (j-model->radius)*(width(in)-2*model->radius) ] <<= 1;
    }
    //done
}

static void lbp_cell_histogram(int *lbp_feat, float *feature, float *cell_sum, int width, int height, int wi, int hj, struct lbp_parameters_t *model) 
{

    uint32_t i,j, bi,bj, idx;
    // TODO: remove this
    if(width == height) {

    }

    int cell_in_col = (model->win_width -2*model->radius) / model->b_size[1];

    lbp_feat += wi+hj*(width-2*model->radius);
    for(j=0; j < model->win_height-2*model->radius; j++) 
    {
        bj = (j) / (model->b_size[1]) - 0.5f;
        for(i=0; i < model->win_width -2*model->radius; i++) 
        {
            bi = (i) / (model->b_size[0]) - 0.5f;

            idx = model->hist_bin_size*(bi+bj*cell_in_col);
            feature[ idx + lbp_feat[i + j*(width -2*model->radius)] ] += 1.0f;
            cell_sum[ bi+bj*cell_in_col ] += 1.0f;
        }
    }
}

return_t lbp_image(matrix_t *in, matrix_t *out, struct lbp_parameters_t *model) 
{

    int feat_in_row = lbp_height(model);
    int feat_in_col = lbp_width(model);
    // update the size of the output if needed
    return_t ret_val = matrix_resize(out, feat_in_row, feat_in_col, 1);
    check_return(ret_val, ret_val);
    /// if the struct lbp_t is called with the deafault value, use optimized version, else use extented struct lbp_t
    if(model->radius == 1 && model->neighbors == 8) 
    {
        lbp_3x3(in, out->_data);
    } else 
    {
        lbp_nxn(in, out->_data, model);
    }
    return SUCCESS;
}

return_t lbp_extract(matrix_t *in, struct feature_t *par_model, float *feature) 
{

    struct lbp_parameters_t *model = par_model->parameters;
    uint32_t i,j, bin;
    matrix_t *out = matrix_create(int32_t);
    return_t ret_val = lbp_image(in, out, model);
    check_return(ret_val, ret_val);

    int32_t *lbp_feat = data(int32_t, out);
    /// if uniform mode is on, compute the scores
    if(model->uniform != model->neighbors) 
    {
        /// use the look up table for uniform struct lbp_t
        if(model->table != NULL) {
            for(i=0; i < volume(out); i++) { lbp_feat[i] = model->table[ lbp_feat[i] ]; }
        }
    }
    uint32_t cell_in_row = rows(out) / model->b_size[0];
    uint32_t cell_in_col = cols(out) / model->b_size[1];

    memset(feature, 0, model->feature_size*sizeof(float));
    float *cell_sum = (float*) calloc(cell_in_row*cell_in_col, sizeof(float));

    /// uniform lbp computed, now create histogram vector for each cell
    lbp_cell_histogram(lbp_feat, feature, cell_sum, model->win_width, model->win_height, 0,0, model);
/*
    for(j=0; j < feat_in_row; j++) {
        int bj = (j) / (model->b_size[0]) - 0.5f;
        for(i=0; i < feat_in_col; i++) {
            int bi = (i) / (model->b_size[1]) - 0.5f;

            int idx = model->hist_bin_size*(bi+bj*cell_in_col);
            //printf("IDX: %d   %d--%d    %d--%d\n", idx, bi, cell_in_col, bj, cell_in_row);
            feature[ idx + lbp_feat[i + j*feat_in_col] ] += 1.0f;
            cell_sum[ bi+bj*cell_in_col ] += 1.0f;
        }
    }
*/

    /// now normalize the cell
    for(j=0; j < cell_in_row; j++) 
    {
        for(i=0; i < cell_in_col; i++) 
        {
            int idx = model->hist_bin_size*(i+j*cell_in_col);
            float norm = 1.0f / (imlab_epsilon + cell_sum[ i+j*cell_in_col ]);
            for(bin=0; bin < model->hist_bin_size; bin++) 
            {
                feature[ idx + bin ] = feature[ idx + bin ] * norm;
            }
        }
    }

    free(lbp_feat);
    free(cell_sum);
    //done
    return SUCCESS;
}

matrix_t *lbp2image(float *feature, struct feature_t *par_model)
{
    // get the LBP model
    struct lbp_parameters_t *model = par_model->parameters;

    // compute the cell in single row and cell in single column
    uint32_t cell_in_row = lbp_height(model) / model->b_size[0];
    uint32_t cell_in_col = lbp_width(model) / model->b_size[1];

    uint32_t output_cell_size = 16;

    // compute the necessary image size
    uint32_t image_width = output_cell_size * cell_in_col;
    uint32_t image_height = output_cell_size * cell_in_row;

    // create the output image
    matrix_t *image = matrix_create(uint8_t, image_height, image_width, 3);

    // check that the necessary memories are allocated
    check_null(image, matrix_null());
    
    uint32_t i, j, bin;

    /// now normalize the cell
    for (j = 0; j < cell_in_row; j++)
    {
        for (i = 0; i < cell_in_col; i++)
        {
            // find the most occured two patterns in the cell
            uint32_t idx = model->hist_bin_size * (i + j * cell_in_col);

            uint32_t bin1 = 0;
            uint32_t bin2 = 0;

            // search for the maximum
            for (bin = 0; bin < model->hist_bin_size; bin++)
            {
                if(feature[idx + bin] >= feature[idx + bin1])
                {
                    bin2 = bin1;
                    bin1 = bin;
                }
            }

            // find the color
            struct color_t b1 = HSV(map(bin1, 0, model->hist_bin_size - 1, 0, 255), 200, 200);
            struct color_t b2 = HSV(map(bin2, 0, model->hist_bin_size - 1, 0, 255), 200, 200);

            // now colorize the output image
            uint32_t x, x1 = i * output_cell_size;
            uint32_t y, y1 = j * output_cell_size;

            // continue if empty or too small
            if( (feature[idx + bin1] + feature[idx + bin2]) < 1e-5)
            {
                continue;
            }

            uint32_t t = output_cell_size * (feature[idx + bin1] / (feature[idx + bin1] + feature[idx + bin2]));

            // fill the grid
            for (y = 0; y < output_cell_size; y++)
            {
                for (x = 0; x < t; x++)
                {
                    atui8(image, y + y1, x + x1, 0) = b1.blue;
                    atui8(image, y + y1, x + x1, 1) = b1.green;
                    atui8(image, y + y1, x + x1, 2) = b1.red;
                }
                for (x = t; x < output_cell_size; x++)
                {
                    atui8(image, y + y1, x + x1, 0) = b2.blue;
                    atui8(image, y + y1, x + x1, 1) = b2.green;
                    atui8(image, y + y1, x + x1, 2) = b2.red;
                }
            }
        }
    }

    //done
    return image;
}

/*
static void lbp_cell_normalize(float *feature, float *cell_sum, int cell_in_row, int cell_in_col, int hist_bin_size) {

    int ci,cj, bin;
    float norm;
    for(cj=0; cj < cell_in_row; cj++) {
        for(ci=0; ci < cell_in_col; ci++) {
            norm = 1.0f/ (imlab_epsilon + *cell_sum++);
            for(bin=0; bin < hist_bin_size; bin++) {
                *feature++ *= norm;
            }
        }
    }
}
*/
/*
void lbp_detect(matrix_t *in, struct lbp_parameters_t *model, struct glm_t *net, float threshold, matrix_t *out) {

    int i,j, ci,cj,bin;
    // detection options
    static int MaxDet  = 1024;
    int WS, WindowSize = 20;
    float ScaleFactor  = 1.2;
    int stride = 3;

    matrix_t *gray;
    // convert the input into grayscale
    rgb2gray(in, out);
    // resize the given output matrix
    matrix_resize(out, height(in), width(in), 3);

    //@TODO add support for floating point images
    uint8_t *in_data  = data(uint8_t, in);
    uint8_t *out_data = data(uint8_t, out);

    for(j=0; j < height(out); j++) {
        for(i=0; i < width(out); i++) {
            int idx = i + width(out)*j;
            out_data[ channels(out)*idx + 0 ] = in_data[idx]*0.2;
            out_data[ channels(out)*idx + 1 ] = in_data[idx]*0.2;
            out_data[ channels(out)*idx + 2 ] = in_data[idx]*0.2;
        }
    }

    /// extract the struct lbp_t of the whole image
    int feat_in_row = height(in)-2*model->radius;
    int feat_in_col = width(in) -2*model->radius;

    int cell_in_row = (model->win_height-2*model->radius) / model->b_size[1];
    int cell_in_col = (model->win_width -2*model->radius) / model->b_size[0];
    // output lbp feature
    int *lbp_feat = (int*) calloc( feat_in_row*feat_in_col,sizeof(int) );
    float *cell_sum = (float*) calloc(cell_in_row*cell_in_col, sizeof(float));

    /// if the struct lbp_t is called with the deafault value, use optimized version, else use extented struct lbp_t
    if(model->radius == 1 && model->neighbors == 8) {
        lbp_3x3(in, lbp_feat);
    } else {
        lbp_nxn(in, lbp_feat, model);
    }
    /// if uniform mode is on, compute the scores
    if(model->uniform != model->neighbors) {
        /// use the look up table for uniform struct lbp_t
        if(model->table != NULL) {
            for(i=0; i < feat_in_row*feat_in_col; i++) { lbp_feat[i] = model->table[ lbp_feat[i] ]; }
        }
    }

    matrix_t *feature = matrix_create(float, 1,model->feature_size);
    float *feature_data = data(float, feature);

    float label;
    //#pragma omp parallel for private(i,j, label)
    for(j=0; j < height(in)-model->win_height; j+=stride) {
        //printf("Progress: %3.2f\r", 100.0f*j/(imge->height-model->height));
        for(i=0; i < width(in)-model->win_width; i+=stride) {

            memset(feature_data, 0, model->feature_size*sizeof(float));
            memset(cell_sum, 0, cell_in_row*cell_in_col*sizeof(float));

            /// extract the histogram for the cells
            lbp_cell_histogram(lbp_feat, feature_data, cell_sum, width(in), height(in), i,j, model);
            lbp_cell_normalize(feature_data, cell_sum, cell_in_row, cell_in_col, model->hist_bin_size);

            glm_predict(feature, &label, net);
            //label = max2(-1, min2(1, label));
            label = label > threshold ? 1:-1;
            //#pragma omp critical

            out_data[ channels(out)*(i+model->win_width/2 + width(out)*(j+model->win_height/2)) + 0 ] += 76*(label+1);
            out_data[ channels(out)*(i+model->win_width/2 + width(out)*(j+model->win_height/2)) + 1 ] += 96*(label+1);
            out_data[ channels(out)*(i+model->win_width/2 + width(out)*(j+model->win_height/2)) + 2 ] += 10*(label+1);

        }
    }
    free(lbp_feat);
    free(cell_sum);
    return;
}
*/
