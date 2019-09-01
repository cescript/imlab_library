#include "private/mlcore_private.h"

struct fern_detector_t {
    // given model
    struct fern_t *model;
    // these variables needs to be computes ones before the
    uint32_t ***selected_features[2]; // [forest_size][tree_depth][a,b][x,y]
};

// create a fern_detector object from the given fern model
struct fern_detector_t *fern_detector_create(struct fern_t *model) {
    model;
}

/*
vector_t *fern_detect(matrix_t *image, struct fern_t *model) {

    // check the input image is allocated
    check_null(image, vector_null());
    // check that the given model is valid
    check_null(model, vector_null());
    check_condition(model->is_trained == 1, "call fern_train before predict", vector_null());
    check_condition(model->user_function != -1, "call fern_set before predict", vector_null());

    // start predictions
    int min_size = 10;
    int max_size = 100;

    uint32_t i,j, p, t, f, forest;;

    // prepare fern model for the current location
    for(j = 0; j < rows(image); j++) {
        for(i = 0; i < cols(image); i++) {



            // find the hash value for the given features
            for(forest = 0; forest < model->forest_size; forest++) {
                uint32_t fern_value = 0;
                for(f = 0; f < model->tree_depth; f++) {
                    fern_value <<= 1;
                    // get the binary feature value
                    fern_value += model->binarize(feature, model->selected_features[forest][f], model->thresholds[forest][f]);
                }
                for(f = 0; f < model->nr_class; f++) {
                    label[f] += model->weights[forest][fern_value][f];
                }
            }




        }
    }






}
*/