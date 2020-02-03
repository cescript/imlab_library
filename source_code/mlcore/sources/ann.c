// bahri abacı ann library
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "private/iocore_private.h" // for getopt
#include "private/mlcore_private.h"

// return the input size of the given layer #l
uint32_t get_input_size(struct ann_t *model, uint32_t layer)
{
    // must be input layer
    if (layer == 0)
    {
        return model->num_feature;
    }
    else
    {
        return model->layer[layer - 1]->num_neuron;
    }
}

// return the output size of the given layer #l
uint32_t get_output_size(struct ann_t *model, uint32_t layer)
{
    // must be output layer
    if (layer == model->num_layer - 1)
    {
        return model->num_class;
    }
    else
    {
        return model->layer[layer + 1]->num_neuron;
    }
}

struct layer_t *layer_create(uint32_t num_input, uint32_t num_neuron, char layer_type)
{
    // create a single layer to return
    struct layer_t *out = (struct layer_t *)calloc(1, sizeof(struct layer_t));

    // get the space wrt density of the layer
    out->num_neuron = num_neuron;
    out->type = layer_type;

    // set the activation function and its derivative wrt given choice
    // logistic function
    if (out->type == 'L')
    {
        out->activation = &sigmoid;
        out->derivative = &sigmoid_d;
    }
    // tangent hyperbolic function
    else if (out->type == 'T')
    {
        out->activation = &tanhyper;
        out->derivative = &tanhyper_d;
    }
    // rectified linear unit function
    else if (out->type == 'R')
    {
        out->activation = &relu;
        out->derivative = &relu_d;
    }
    // default value is logistic function
    else
    {
        out->activation = &sigmoid;
        out->derivative = &sigmoid_d;
    }

    // we allocate all the spaces that we need
    out->bias = (float *)calloc(out->num_neuron, sizeof(float));
    out->weights = (float **)calloc(out->num_neuron, sizeof(float *));

    out->net_inputs = (float *)calloc(out->num_neuron, sizeof(float));
    out->outputs = (float *)calloc(out->num_neuron, sizeof(float));

    // get space for the weights of each neuron
    int i, j;
    for (i = 0; i < out->num_neuron; i++)
    {
        out->bias[i] = random_float(-1, 1);
        out->weights[i] = (float *)calloc(num_input, sizeof(float));

        // if the space allocated, fill the weights
        if (out->weights[i] != NULL)
        {
            // fill weights with random numbers
            for (j = 0; j < num_input; j++)
            {
                out->weights[i][j] = random_float(-1, 1);
            }
        }
        else
        {
            // error
        }
    }

    return out;
}

// create function for the ann_t class
// ann(LOGLOSS, "5L,3T,5S")
struct ann_t *ann_create(const char *layers, char options[])
{
    // allocate memory for the output network
    struct ann_t *model = (struct ann_t *)malloc(sizeof(struct ann_t));

    model->max_iter = 1000;
    model->batch_size = 1;
    model->epsilon = 0.001f;
    model->eta = 0.001f;

    // get values from configuration
    getopt(uint32_t, options, "max_iter", &model->max_iter);
    getopt(uint32_t, options, "batch_size", &model->batch_size);
    getopt(float, options, "eta", &model->eta);
    getopt(float, options, "epsilon", &model->epsilon);

    // allocate enough space for the layer type and number of neurons
    model->layer_type = (char *)calloc(strlen(layers), sizeof(char));
    model->num_neuron = (uint32_t *)calloc(strlen(layers), sizeof(uint32_t));

    // create the layers
    model->num_feature = 0;
    model->num_class = 0;

    model->num_layer = 0;
    model->layer = NULL;

    // parse the layers
    char *layer_start = imlab_strdup(layers);
    char *layer = strtok(layer_start, "-");
    while (layer != NULL)
    {
        int n = sscanf(layer, "%d%c", &model->num_neuron[model->num_layer], &model->layer_type[model->num_layer]);
        if (n == 2)
        {
            model->num_layer++;
        }
        else
        {
            message(WARNING, "incorrect layer descriptor");
        }

        layer = strtok(NULL, "-");
    }
    free(layer_start);

    if (model->num_layer == 0)
    {
        message(WARNING, "incorrect layer size");
    }
    // allocate if necessary
    else
    {
        model->layer = (struct layer_t **)calloc(model->num_layer, sizeof(struct layer_t *));
    }
    
    // return the model
    return model;
}

// print the information about the model
void ann_view(struct ann_t *model)
{
    printf("Parameters of the Artificial Neural Network\n");
    printf("Options:\n");
    printf("> MaxIter      : %d\n", model->max_iter);
    printf("> Batch Size   : %d\n", model->batch_size);
    printf("> Eta          : %3.5f\n", model->eta);
    printf("> Epsilon      : %3.5f\n", model->epsilon);

    // print the layer configuration
    if (model->num_layer > 1)
    {
        printf("> Hidden Layer : %d\n", model->num_layer - 1);
        int i;
        for (i = 0; i < model->num_layer - 1; i++)
        {
            printf("  %d. Layer (%c) has %d neurons\n", i + 1, model->layer_type[i], model->num_neuron[i]);
        }
        printf("\n");
    }

    // continue if the training is done
    if (model->num_feature == 0)
    {
        return;
    }

    printf("Training results:\n");
    printf("> #Input Feature : %d\n", model->num_feature);
    printf("> #Output Class  : %d\n", model->num_class);
}

// train the network with the given inputs and net configurations
return_t ann_train(matrix_t *input, matrix_t *output, struct ann_t *model)
{
    int cond0 = is_numeric(input) && is_numeric(output);
    check_condition(cond0, ERROR_TYPE_MISMATCH, "input must be a numeric matrix");

    int cond1 = rows(input) == rows(output);
    check_condition(cond1, ERROR_DIMENSION_MISMATCH, "input and label matrice have different number of samples");

    int cond2 = channels(input) == 1 && channels(output) == 1;
    check_condition(cond2, ERROR_DIMENSION_MISMATCH, "arguments must be one-dimensional matrice");

    int cond3 = model->num_layer > 0;
    check_condition(cond3, ERROR_DIMENSION_MISMATCH, "layer size must be positive");

    // we are ready to update the model
    uint32_t numSample = rows(input);
    model->num_feature = cols(input);
    model->num_class = cols(output);

    // check that the given model can be used
    int cond4 = model->num_neuron[model->num_layer - 1] == model->num_class;
    check_condition(cond4, ERROR_DIMENSION_MISMATCH, "number of output neurons doesnt match the number of output classes");

    // before everything we will compute the number of inputs for the layer and allocate space for the computations
    // output of the nput layer will be the input of the first layer
    uint32_t iter, b, i, j, l; // iteration, batch element, num neuron, num inputs, layer
    for (l = 0; l < model->num_layer; l++)
    {
        model->layer[l] = layer_create(get_input_size(model, l), model->num_neuron[l], model->layer_type[l]);
    }

    // allocate memory for the backpropagation
    float **delta = (float **)calloc(model->num_layer, sizeof(float *));
    for (l = 0; l < model->num_layer; l++)
    {
        delta[l] = (float *)calloc(model->layer[l]->num_neuron, sizeof(float));
    }

    // allocate memory to store weight change
    float **bias_update = (float **)calloc(model->num_layer, sizeof(float *));
    float ***weight_update = (float ***)calloc(model->num_layer, sizeof(float **));
    for (l = 0; l < model->num_layer; l++)
    {
        bias_update[l] = (float *)calloc(model->layer[l]->num_neuron, sizeof(float));
        weight_update[l] = (float **)calloc(model->layer[l]->num_neuron, sizeof(float *));

        // allocate weight update array for each input
        for (i = 0; i < model->layer[l]->num_neuron; i++)
        {
            weight_update[l][i] = (float *)calloc(get_input_size(model, l), sizeof(float));
        }
    }

    // start iterations
    for (iter = 0; iter < model->max_iter; iter++)
    {
        // shuffle the dataset
        uint32_t *idx = random_permutation(numSample);

        // set the remaining data size
        uint32_t remainingSampleSize = numSample;
        uint32_t sampleIndexFromStart = 0;

        while (remainingSampleSize != 0)
        {
            // find the current batch size
            uint32_t batchSize = remainingSampleSize > model->batch_size ? model->batch_size : remainingSampleSize;

            // for all training sample
            for (b = 0; b < batchSize; b++)
            {
                // pick a random people in the database
                uint32_t s = idx[b + sampleIndexFromStart];

                // get pointer to the actual input and output values
                float *inputs = data(float, input, s, 0);
                float *target = data(float, output, s, 0);

                // FEED FORWARD
                // feed forward based on that input
                for (l = 0; l < model->num_layer; l++)
                {
                    uint32_t input_size = get_input_size(model, l);

                    // for each neuron in the hidden layer
                    //#pragma omp parallel for private(j)
                    for (i = 0; i < model->layer[l]->num_neuron; i++)
                    {
                        // compute the neuron net input
                        double neuron_signal = model->layer[l]->bias[i];
                        //#pragma omp parallel for reduction(+:neuron_signal)
                        for (j = 0; j < input_size; j++)
                        {
                            neuron_signal += model->layer[l]->weights[i][j] * inputs[j];
                        }

                        // set the net input and outputs for the current layer
                        model->layer[l]->net_inputs[i] = neuron_signal;
                        model->layer[l]->outputs[i] = model->layer[l]->activation(neuron_signal);
                    }

                    // set the inputs of the next layer as the output of the current layer
                    inputs = model->layer[l]->outputs;
                }

                // BACKPROPAGATION
                // now update the weights using the gradent changes over the all network
                for (l = model->num_layer; l-- > 0;)
                {
                    uint32_t input_size = get_input_size(model, l);
                    uint32_t output_size = get_output_size(model, l);

                    // if the layer is the last layer, than compute the error
                    if (l == model->num_layer - 1)
                    {
                        // for each neuron in the output layer
                        //#pragma omp parallel for
                        for (i = 0; i < model->layer[l]->num_neuron; i++)
                        {
                            delta[l][i] = model->layer[l]->derivative(model->layer[l]->net_inputs[i]) * (model->layer[l]->outputs[i] - target[i]);
                        }
                    }
                    // propagate the output error to the hidden layers
                    else
                    {
                        // for each neuron in the hidden layer
                        //#pragma omp parallel for private(j)
                        for (i = 0; i < model->layer[l]->num_neuron; i++)
                        {
                            // compute the effect of ith neuron on the output error
                            double neuron_error = 0;
                            //#pragma omp parallel for reduction(+:neuron_error)
                            for (j = 0; j < output_size; j++)
                            {
                                neuron_error += model->layer[l + 1]->weights[j][i] * delta[l + 1][j];
                            }

                            // compute the gradient
                            delta[l][i] = model->layer[l]->derivative(model->layer[l]->net_inputs[i]) * neuron_error;
                        }
                    }

                    // set the inputs for the weight update
                    inputs = l == 0 ? data(float, input, s, 0) : model->layer[l - 1]->outputs;

                    // update the weights
                    for (j = 0; j < model->layer[l]->num_neuron; j++)
                    {
                        // update the biases
                        bias_update[l][j] += delta[l][j];

                        // update the weights
                        for (i = 0; i < input_size; i++)
                        {
                            weight_update[l][j][i] += delta[l][j] * inputs[i];
                        }
                    }
                }
            }

            // allow weight update after the whole batch is processed
            for (l = 0; l < model->num_layer; l++)
            {
                uint32_t input_size = get_input_size(model, l);

                for (j = 0; j < model->layer[l]->num_neuron; j++)
                {
                    // update the biases
                    model->layer[l]->bias[j] -= (model->eta * bias_update[l][j]) / batchSize;
                    bias_update[l][j] = 0;

                    // update the weights
                    for (i = 0; i < input_size; i++)
                    {
                        model->layer[l]->weights[j][i] -= (model->eta * weight_update[l][j][i]) / batchSize;
                        weight_update[l][j][i] = 0;
                    }
                }
            }

            // update the remaining sample size for the next iterations
            remainingSampleSize -= batchSize;
            sampleIndexFromStart += batchSize;
        }

        // free shuffled indices
        free(idx);
    }

    // TRAINING IS DONE

    // free all the variables allocated for training
    // free memory allocated for delta
    for (l = 0; l < model->num_layer; l++)
    {
        free(delta[l]);
    }
    free(delta);

    // free the weight update arrays
    for (l = 0; l < model->num_layer; l++)
    {
        // allocate weight update array for each input
        for (i = 0; i < model->layer[l]->num_neuron; i++)
        {
            free(weight_update[l][i]);
        }

        free(weight_update[l]);
        free(bias_update[l]);
    }
    free(weight_update);
    free(bias_update);
}

// train the network with the given inputs and net configurations
return_t ann_predict(matrix_t *input, matrix_t *output, struct ann_t *model)
{
    check_null(model);

    int cond0 = cols(input)  == model->num_feature;
    check_condition(cond0, ERROR, "given model has not been trained for the current input");

    int cond1 = is_numeric(input) && is_numeric(output);
    check_condition(cond1, ERROR_TYPE_MISMATCH, "input must be a numeric matrix");

    int cond2 = channels(input) == 1;
    check_condition(cond2, ERROR_DIMENSION_MISMATCH, "input must be one-dimensional matrix");

    // resize the ouput matrix
    matrix_resize(output, rows(input), model->num_class, 1);

    // start iterations here
    int s, l, i, j;

    // for all training sample
    for (s = 0; s < rows(input); s++)
    {
        // feed forward based on that input
        float *inputs = data(float, input, s, 0);
        float *outputs = data(float, output, s, 0);

        for (l = 0; l < model->num_layer; l++)
        {
            uint32_t input_size = get_input_size(model, l);

            // for each neuron in the hidden layer
            for (i = 0; i < model->layer[l]->num_neuron; i++)
            {
                // compute the neuron net input
                double neuron_signal = model->layer[l]->bias[i];
                for (j = 0; j < input_size; j++)
                {
                    neuron_signal += model->layer[l]->weights[i][j] * inputs[j];
                }
                
                // set the net input and outputs for the current layer
                model->layer[l]->net_inputs[i] = neuron_signal;
                model->layer[l]->outputs[i] = model->layer[l]->activation(neuron_signal);
            }

            // set the inputs of the next layer as the output of the current layer
            inputs = model->layer[l]->outputs;
        }

        // here the inputs array keeps the pointer to the predicted values
        memcpy(outputs, inputs, model->num_class * sizeof(float));
    }
}

/// struct ann_t model import/export functions
struct ann_t *ann_read(const char *filename)
{
    // create the file with the given key value pair
    FILE *fp = imlab_fopen(filename, "rb", "ann");
    check_file(fp, NULL);

    // create input net
    struct ann_t *net = (struct ann_t *)calloc(1, sizeof(struct ann_t));

    // now read the options
    fread(&net->max_iter, sizeof(uint32_t), 1, fp);
    fread(&net->batch_size, sizeof(uint32_t), 1, fp);
    fread(&net->epsilon, sizeof(float), 1, fp);
    fread(&net->eta, sizeof(float), 1, fp);

    // read the array sizes
    fread(&net->num_feature, sizeof(uint32_t), 1, fp);
    fread(&net->num_class, sizeof(uint32_t), 1, fp);
    fread(&net->num_layer, sizeof(uint32_t), 1, fp);

    // allocate spaces
    if (net->num_layer > 0)
    {
        net->layer_type = (char *)calloc(net->num_layer, sizeof(char));
        net->num_neuron = (uint32_t *)calloc(net->num_layer, sizeof(uint32_t));
        net->layer = (struct layer_t **)calloc(net->num_layer, sizeof(struct layer_t *));

        // now read the number of neurons and layer types
        fread(net->layer_type, sizeof(char), net->num_layer, fp);
        fread(net->num_neuron, sizeof(uint32_t), net->num_layer, fp);

        // read the layers
        uint32_t l, i;
        for (l = 0; l < net->num_layer; l++)
        {
            uint32_t input_size = get_input_size(net, l);

            // create a layer
            net->layer[l] = layer_create(input_size, net->num_neuron[l], net->layer_type[l]);

            // read bias and weights
            fread(net->layer[l]->bias, sizeof(float), net->layer[l]->num_neuron, fp);

            for (i = 0; i < net->layer[l]->num_neuron; i++)
            {
                fread(net->layer[l]->weights[i], sizeof(float), input_size, fp);
            }
        }
    }
    else
    {
        net->layer_type = NULL;
        net->num_neuron = NULL;
        net->layer = NULL;
    }

    // close the file
    fclose(fp);

    //done
    return net;
}

return_t ann_write(struct ann_t *net, const char *filename)
{
    // open the file with the given key value pair
    FILE *fp = imlab_fopen(filename, "wb", "ann");
    check_file(fp);

    // now read the options
    fwrite(&net->max_iter, sizeof(uint32_t), 1, fp);
    fwrite(&net->batch_size, sizeof(uint32_t), 1, fp);
    fwrite(&net->epsilon, sizeof(float), 1, fp);
    fwrite(&net->eta, sizeof(float), 1, fp);

    // read the array sizes
    fwrite(&net->num_feature, sizeof(uint32_t), 1, fp);
    fwrite(&net->num_class, sizeof(uint32_t), 1, fp);
    fwrite(&net->num_layer, sizeof(uint32_t), 1, fp);

    if(net->num_layer > 0)
    {
        // now read the number of neurons and layer types
        fwrite(net->layer_type, sizeof(char), net->num_layer, fp);
        fwrite(net->num_neuron, sizeof(uint32_t), net->num_layer, fp);

        // read the layers
        uint32_t l, i;
        for (l = 0; l < net->num_layer; l++)
        {
            uint32_t input_size = get_input_size(net, l);

            // read bias and weights
            fwrite(net->layer[l]->bias, sizeof(float), net->layer[l]->num_neuron, fp);

            for (i = 0; i < net->layer[l]->num_neuron; i++)
            {
                fwrite(net->layer[l]->weights[i], sizeof(float), input_size, fp);
            }
        }
    }

    // close the file
    fclose(fp);

    // done return
    return SUCCESS;
}
