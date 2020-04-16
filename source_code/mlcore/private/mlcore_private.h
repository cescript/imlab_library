//
// Created by cescript on 8.01.2018.
//

#ifndef IMLAB_MLCORE_PRIVATE_H
#define IMLAB_MLCORE_PRIVATE_H

#include "private/core_private.h"
#include "../mlcore.h"

struct sgd_parameters 
{
    uint32_t solver;
    // training features
    uint32_t max_iter;
    float epsilon;
    float eta;
    float lambda;
};

// glm_t output
struct glm_t
{
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

struct fern_t 
{

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

struct q_table_t
{
    float gamma; // discount factor

    uint32_t num_states;
    uint32_t num_actions;
    float **qtable;
};

struct layer_t
{
    // type of the neuron
    char type;
    uint32_t num_neuron;

    // activation function and derivative of it
    float (*activation)(float); // sigmoid-tangent-relu
    float (*derivative)(float); // derivative of the above activation functions

    // for each input we should have a weight and one bias
    float **weights;

    float *net_inputs;
    float *outputs;
};

struct ann_t
{
    // options for the learning
    // training features
    uint32_t max_iter;
    uint32_t batch_size; // size of the batch for stochastic gradient descent
    float epsilon;
    float eta;

    uint32_t optimizer; // optimizer type
    float momentum;// using in gradient direction calculations
    float rho;     // used in RMSprop and Adadelta and ADAM
    float beta; // used in ADAM

    // structure of the layer
    char *layer_type;
    uint32_t *num_neuron;

    // all the variables thta we need
    uint32_t num_feature; // num input
    uint32_t num_class;   // num output
    uint32_t num_layer;   // number of layers

    // layer structure
    struct layer_t **layer;
};

// COMMON UTILITY FUNCTIONS
// logistic function and its derivatives
float sigmoid(float x);
float sigmoid_d(float x);

// tangent hyperbolic function and its derivative
float tanhyper(float x);
float tanhyper_d(float x);

// relu function and its derivative
float relu(float x);
float relu_d(float x);

#endif //IMLAB_MLCORE_PRIVATE_H
