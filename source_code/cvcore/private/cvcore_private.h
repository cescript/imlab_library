#ifndef IMLAB_CVCORE_PRIVATE_H
#define IMLAB_CVCORE_PRIVATE_H

#include "../cvcore.h"
#include "../../core/private/core_private.h"
#include "../../iocore/private/iocore_private.h"
#include "../../imcore/private/imcore_private.h"
#include "../../mlcore/private/mlcore_private.h"
#include "../../lacore/private/lacore_private.h"

struct encoder_parameters_t {
    int type;

    uint32_t width;
    uint32_t height;
    uint32_t feature_size;
};

return_t imencode_gray(matrix_t *in, float *feature);
struct feature_t *encoder_create(int width, int height, char options[]);
return_t encoder_extract(matrix_t *in, struct feature_t *par_model, float *feature);

struct lbp_parameters_t {
    uint32_t radius;
    uint32_t neighbors;
    uint32_t uniform;
    uint32_t b_size[2];
    // computed parameters
    uint32_t win_width;
    uint32_t win_height;
    uint32_t feature_size;
    uint32_t hist_bin_size;
    uint32_t *table;
};

struct feature_t *lbp_create(int width, int height, char options[]);
void lbp_view(struct feature_t *model);
uint32_t  lbp_feature_size(struct feature_t *model);
uint32_t  lbp_width(struct lbp_parameters_t *model);
uint32_t  lbp_height(struct lbp_parameters_t *model);

return_t lbp_image(matrix_t *in, matrix_t *out, struct lbp_parameters_t *model);
return_t lbp_extract(matrix_t *in, struct feature_t *model, float *feature);
void lbp_detect(matrix_t *in, struct lbp_parameters_t *model, struct glm_t *net, float threshold, matrix_t *out);


struct hog_parameters_t {
    uint32_t nbins;
    uint32_t b_size[2];
    uint32_t c_size[2];
    uint32_t stride[2];
    // input dependent parameters (do not play with these)
    uint32_t width, hog_width, block_width;
    uint32_t height,hog_height,block_height;
    uint32_t feature_size;
    // these arrays will be used many times, so allocating them here will save some time during the operation
    float *cell_hist;
    float *grad_mag;
    float *grad_ori;
};

/**
 * Create a new HOG model from the given inputs.
 * @param width Width of the image that the features will be extracted.
 * @param height Height of the image that the features will be extracted.
 * @param options Optional parameters in the IMLAB parameter format.
 * @return Created feature model for the later usage.
 */
struct feature_t *hog_create(uint32_t width, uint32_t height, char options[]);
/**
 * Displays the created HOG model.
 * @param model Created HOG model which will be displayed.
 */
void hog_view(struct feature_t *model);
/**
 * Return the HOG feature vector size.
 * @param model Created HOG model to be used for feature extraction.
 * @return Returns the feature vector size for the given model.
 */
uint32_t  hog_feature_size(struct feature_t *model);
/**
 * Extracts the HOG features from the given image using the previously created model.
 * @param in Input image to be used for feature extraction
 * @param feature Output vector where the features will be written. It should be allocated before calling this function.
 * @param model Created HOG model to be used for feature extraction.
 * @return SUCCESS if the features are extracted correctly or the causing error.
 */
return_t hog_extract(matrix_t *in, struct feature_t *model, float *feature);
//matrix_t* hog_detect(matrix_t *in, struct hog_parameters_t *model, struct glm_t *net, float threshold);
matrix_t *hog2image(float *feature, struct hog_parameters_t *model);

void lbp_3x3(matrix_t *in, int *lbp_feat);
void lbp_nxn(matrix_t *in, int *lbp_feat, struct lbp_parameters_t *model);

struct haar_feature_t
{
    int tilt;

    // rectangles under a feature
    uint32_t length;
    struct rectangle_t *rects;
};

struct haar_tree_t
{
    double threshold;
    double l_val;
    double r_val;

    struct haar_feature_t *feats;
};

struct haar_stage_t
{
    double stage_threshold;
    int parent;
    int next;

    int length;
    struct haar_tree_t **trees;
};

typedef struct haar_t
{
    int size1;
    int size2;

    int length;
    struct haar_stage_t **stages;
} haar_t;

#endif