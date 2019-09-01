#include "private/cvcore_private.h"

// create a feature object
struct feature_t *feature_create(enum cv_algorithm_t algorithm, uint32_t  width, uint32_t height, char *options) {

    // create a single instance of feature_t
    struct feature_t *out = NULL;

    if(algorithm == CV_ENCODER) {
        out = encoder_create(width, height, options);
    } else if(algorithm == CV_LBP) {
        out = lbp_create(width, height, options);
    } else if(algorithm == CV_HOG) {
        out = hog_create(width, height, options);
    }
    // return the newly created feature
    return out;
}

void feature_view(struct feature_t *model) {
    // if the input is null, warn the user
    check_null(model, );

    if(model->algorithm == CV_ENCODER) {

    } else if(model->algorithm == CV_LBP) {
        lbp_view(model);
    } else if(model->algorithm == CV_HOG) {
        hog_view(model);
    }
    // return nothing
    return;
}

// return the length of the feature using the model
uint32_t  feature_size(struct feature_t *model) {
    check_null(model, 0);
    return model->feature_size;
}

// do all the controls and run the method
return_t feature_extract(matrix_t *img, struct feature_t *model, float *output) {
    // check that the input is image
    check_image(img, ERROR_TYPE_MISMATCH);
    // check that the model is initialized
    check_null(model, ERROR_NULL_TYPE);

    int cond = width(img) == model->image_width && height(img) == model->image_height;
    check_condition(cond, ERROR_DIMENSION_MISMATCH, "image size and model size or not match");

    // run the method
    return_t ret_val = model->method(img, model, output);
    // check that the method finished succesfully
    return ret_val;
}