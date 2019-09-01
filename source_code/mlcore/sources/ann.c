// bahri abacı ann library
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "private//prcore_private.h"
#include "private//iocore_private.h"

#ifdef USE_ANN

struct sgd_parameters {
    int solver;
    // training features
    int max_iter;
    int batch_size;
    float epsilon;
    float eta;
    float lambda;
};

float sigmoid(float x) { return x; }      //{ return 1/(1+expf(-x)); }
float sigmoid_d(float x) { return x; }


struct layer_t {
    uint32_t type;

    uint32_t num_input;
    uint32_t num_neuron;

    // activation function and derivative of it
    float (*activation)(float); // sigmoid-tangent-max
    float (*derivative)(float); // derivative of the aboves
    // for each input we should have a weight and one bias
    float *inputs;
    float *bias;
    float **weights;
    float *outputs;
};

struct layer_t *layer_create(uint32_t num_input,  uint32_t num_neuron, uint8_t layer_type) {
    // general index counter for the loops
    int i = 0;
    // create a single layer to return
    struct layer_t *out = (struct layer_t*) calloc(1, sizeof(struct layer_t));
    // get the space wrt density of the layer
    out->num_input     = num_input;
    out->num_neuron    = num_neuron;
    // set the activation function and its derivative wrt given choice
    if(layer_type == 'L') {
        out->activation = &sigmoid;
        out->derivative = &sigmoid_d;
    } else {
        out->activation = &sigmoid;
        out->derivative = &sigmoid_d;
    }
    // we allocate all the spaces that we need
    out->bias = (float*) calloc(out->num_neuron, sizeof(float));
    out->weights = (float**) calloc(out->num_neuron, sizeof(float*));
    out->outputs       = (float*) calloc(out->num_neuron, sizeof(float));
    // get space for the weights of each neuron
    for(i=0; i < out->num_neuron; i++) {
        out->weights[i] = (float*) calloc(out->num_input, sizeof(float));
    }

    return out;
}

/*
// since it is input_layer output is equal to input data
feed_forward(float *data, layer_t *input_layer, float *output);

// since it is a layer output is equal to activation(data * neuron_weight + bias)
feed_forward(float *data, layer_t *layer, float *output);

// since it is an output_layer output is equal to data * neuron_weight + bias
feed_forward(float *data, layer_t *output_layer, float *output);
*/
struct ann_t {
    // options for the learning
    struct sgd_parameters options;
    // structure of the layer
    char *layer_type;
    uint32_t *num_neuron;
    void (*update_cost)(float*, float*, float*, uint32_t);
    // all the variables thta we need
    uint32_t num_feature; // num input
    uint32_t num_class;   // num output
    uint32_t num_layer;   // number of layers
    // layer structure
    struct layer_t **layer;
};



// create function for the ann_t class
// ann(LOGLOSS, "5L,3T,5S")
struct ann_t *ann(uint32_t solver, const char *layers, char options[]) {
    // allocate memory for the output network
    struct ann_t *model = (struct ann_t*) malloc(sizeof(struct ann_t));

    model->options.solver = solver;
    model->options.max_iter = 1000;
    model->options.batch_size = 100;
    model->options.epsilon = 0.001f;
    model->options.eta = 0.001f;

    // get values from configuration
    getopt(int, options, "max_iter", &model->options.max_iter);
    getopt(int, options, "batch_size", &model->options.batch_size);
    getopt(float, options, "epsilon", &model->options.epsilon);
    getopt(float, options, "eta", &model->options.eta);

    model->layer_type = (uint8_t*)  calloc(strlen(layers), sizeof(uint8_t));
    model->num_neuron = (uint32_t*) calloc(strlen(layers), sizeof(uint32_t));
    // create the layers
    model->num_feature = 0;
    model->num_class   = 0;

    // create a suitable error function
    model->update_cost;

    uint32_t l = 0;
    char *layer_start = strdup(layers);
    char *layer = strtok (layer_start,"-");
    while (layer != NULL) {
        sscanf(layer, "%d%c", &model->num_neuron[l], &model->layer_type[l]);
        l++;
        layer = strtok (NULL, "-");
    }
    // allocate space for the laeyrs
    model->num_layer = l;
    model->layer = (struct layer_t**) calloc(model->num_layer, sizeof(struct layer_t*));
    // free the allocated memory for the configuration
    free(layer_start);

    return model;
}

// print the information about the model
void ann_view(struct ann_t *model) {
    //
    int i;
    printf("Parameters of the Artificial Neural Network\n");
    printf("Options:\n");
    printf("> Solver     : %d\n", model->options.solver);
    printf("> MaxIter    : %d\n", model->options.max_iter);
    printf("> Batch Size : %d\n", model->options.batch_size);
    printf("> Eta        : %3.5f\n", model->options.eta);
    printf("> Epsilon    : %3.5f\n", model->options.epsilon);
    // print the layer configuration
    printf("> #Hidden Layer : %d\n", model->num_layer);
    for(i=0; i < model->num_layer; i++) {
        printf("  %d. Layer (%c) has %d neurons\n", i+1, model->layer_type[i], model->num_neuron[i]);
    }
    printf("\n");
    // continue if the training is done
    if(model->num_feature == 0) { return; }

    printf("Training results:\n");
    printf("> #Input Layer  : %d\n", model->num_feature);
    printf("> #Output Layer : %d\n", model->num_class);

}

// train the network with the given inputs and net configurations
int ann_train(matrix_t *input, matrix_t *output, struct ann_t *model) {
    //This function assumes that the given input is either y in [-1,1] or y={-1,1}
    if(rows(input) != rows(output)) {
        print_error("%s: input and output matrice have different number of samples!\n",__func__);
        return 0;
    }
    if(! (channels(input) == 1 || channels(output) == 1) ) {
        print_error("%s: expects its arguments to be one-dimensional matrice!\n",__func__);
    }

    // we are ready to update the model
    uint32_t numSample = rows(input);
    model->num_feature = cols(input);
    model->num_class   = cols(output);
    model->layer       = (struct layer_t**) calloc(model->num_layer + 1, sizeof(struct layer_t*));
    // before everything we will compute the number of inputs for the layer and allocate space for the computations
    int l;
    // output of the nput layer will be the input of the first layer
    uint32_t next_input = model->num_feature;
    for(l=0; l < model->num_layer; l++) {
        model->layer[l] = layer_create(next_input, model->num_neuron[l], model->layer_type[l]);
        next_input = model->layer[l]->num_neuron;
    }
    // at the output layer at the end
    model->layer[l] = layer_create(next_input, model->num_class, 'O');

    float eps = model->options.epsilon;
    float eta = model->options.eta;
    int nIter = model->options.max_iter;
    int nBatch= model->options.batch_size;

    // start iterations here
    int iter,b,c, i,j;
    uint32_t *idx = random_permutation(numSample);
    // allocate memory for the backporpagation
    float **delta      = (float**) malloc(model->options.batch_size, sizeof(float*));
    for(b=0; b < model->options.batch_size; b++) {
        delta[b] = (float*)  calloc(model->num_class,sizeof(float));
    }
    // start iterations
    for (iter=0; iter < model->options.max_iter; iter++) {
        // feed forward
        for(b=0; b < model->options.batch_size; b++) {
            // pick a random people in the database
            uint32_t s = idx[iter%numSample];
            // feed forward based on that input
            float *inputs = pointer(float, input, s, 0);
            for(l=0; l < model->num_layer + 1; l++) {
                // for each neuron in the hidden layer
                for(i=0; i < model->layer[l]->num_neuron; i++) {
                    double neuron_signal = model->layer[l]->bias[i];
                    for(j=0; j < model->layer[l]->num_input; j++) {
                        neuron_signal += model->layer[l]->weights[i][j] * inputs[j];
                    }
                    model->layer[l]->outputs[j] = model->layer[l]->activation(neuron_signal);
                }
                inputs = model->layer[l]->outputs;
            }
            float *yt = model->layer[l]->outputs;
            float *ys = pointer(float, output, s, 0);
            // update the delta
            for(c=0; c < model->num_class; c++) {
                delta[b] += model->layer[l]->derivative() * (yt[c]-ys[c]);
            }
            // last layer is the output layer, so its outputs are the outputs of the ann
            // TODO: fix this
            expectations[b] = 1;
        }
        //δj = (∂f/∂netj)(yj(t)− yj)

        float *inputs = pointer(float, input, s, 0);
        // now back propagate using the batch error
        for(l=model->num_layer; l > -1; l--) {


            // for each neuron in the hidden layer
            for(i=0; i < model->layer[l]->num_neuron; i++) {

                double neuron_signal = model->layer[l]->bias[i];
                for(j=0; j < model->layer[l]->num_input; j++) {
                    neuron_signal += model->layer[l]->weights[i][j] * inputs[j];
                }
                model->layer[l]->outputs[j] = model->layer[l]->activation(neuron_signal);
            }
            inputs = model->layer[l]->outputs;
    }
    //DONE
}



#define LOGISTIC 1

int main() {

    // create a simple two layer network
    struct ann_t *net = ann(2, "100L-220T-120L-40T", "max_iter:123, batch_size:4, epsilon:0.5, eta:0.06");
    // now we are ready to train the network
    ann_view(net);


    printf("everything ok!\n");
    system("pause");
    return 0;

}

#endif
