//
// Created by cescript on 8.01.2018.
//

#ifndef IMLAB_MLCORE_PRIVATE_H
#define IMLAB_MLCORE_PRIVATE_H

#include "private/core_private.h"
#include "../mlcore.h"

struct sgd_parameters {
    uint32_t solver;
    // training features
    uint32_t max_iter;
    float epsilon;
    float eta;
    float lambda;
};

// glm_t output
struct glm_t{
    // parameters for the linear model
    struct sgd_parameters options;
    //
    uint32_t nr_feature, nr_class;
    // learned parameters
    uint32_t nr_learner;
    float *cost;
    float *bias;
    float **beta;
};

struct fern_t {

    // filled in fern constructor
    uint8_t  solver;
    uint32_t forest_size;
    uint32_t tree_depth, tree_size;
    uint32_t nr_feature; // how many features will be used to binarize the feature
    uint32_t enable_boosting;

    // sets via fern_set
    int8_t user_function; // 0 if the default values are used, or 1 if the user sets the methods, -1 for uninitialized
    void **aux; // auxilary pointers for user inputs, this point can be set before training
    // function pointer to select the good features from the given feature set
    return_t (*select)(float*, float*, float*, void**, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t **, float **, uint32_t);
    uint32_t (*binarize)(float *data, uint32_t *p, float *t);

    // filled inside fern_train function
    uint32_t is_trained;
    uint32_t nr_class;
    uint32_t ***selected_features; // [forest_size][tree_depth][a,b]
    float ***thresholds; // [forest_size][2^tree_depth][nr_feature]
    float ***weights; // [forest_size][2^tree_depth][CLASSIFICATION ? nr_class : nr_learner]


};

#endif //IMLAB_MLCORE_PRIVATE_H
