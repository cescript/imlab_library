// implementation of FERN learning algorithm
#include <stdlib.h>
#include "private/imcore_private.h" // for color_t and HSV
#include "private/iocore_private.h" // for getopt
#include "private/mlcore_private.h"

// random feature selection method for fern train
return_t fern_random_feature_selector(float *feature, float *output, float *current_output, void **aux, uint32_t sample_count, uint32_t feature_count, uint32_t output_length, uint32_t nr_fern, uint32_t **selected_features, float **thresholds, uint32_t forest) {
    // fill selected features
    uint32_t j = 0;
    for(j = 0; j < nr_fern; j++) {
        selected_features[j][0] = (uint32_t )random_int(0, feature_count-1);
        selected_features[j][1] = (uint32_t )random_int(0, feature_count-1);
    }
    // return true
    return SUCCESS;
}

// correlation based feature selection method for fern_train
return_t fern_correlation_based_feature_selector(float *feature, float *output, float *current_output, void **aux, uint32_t sample_count, uint32_t feature_count, uint32_t output_length, uint32_t nr_fern, uint32_t **selected_features, float **thresholds, uint32_t forest) {

    uint32_t f, p, i, j = 0;

    if(forest == 0) {
        forest = 1;
    }

    // do random projection of the current output
    float *random_base = (float*) malloc(output_length * sizeof(float));
    float *projection  = (float*) malloc(sample_count * sizeof(float));
    float *projection_feature_covariance = (float*) malloc(feature_count * sizeof(float));

    // get the transposed features
    float **feature_transpose = (float**) aux[0];
    // get the feature correlation table
    float **covariance_table = (float**) aux[1];

    // start feature selection
    for(f = 0; f < nr_fern; f++) {

        // create a random projection base
        for(j = 0; j < output_length; j++) {
            random_base[j] = random_float(-1,1);
        }
        // project the current output onto the random base
        for(p = 0; p < sample_count; p++) {
            // get pointer to the correct sample
            float *desired = output + p*output_length;
            float *current = current_output + p*output_length;
            // get the projection & current covariance
            for(j = 0; j < output_length; j++) {
                projection[p] += (desired[j] - current[j] / forest) * random_base[j];
            }
        }
        // compute the projection covariance
        float projection_variance = covariance(projection, projection, sample_count);

        // compute covariance between projection and features
        for(i = 0; i < feature_count; i++) {
            // compute the covariance
            projection_feature_covariance[i] = covariance(projection, feature_transpose[i], sample_count);
        }

        // go on every feature pair and find the best pairs
        double correlation = 0, max_correlation = 0;
        uint8_t suspicious = 0;
        for(i = 0; i < feature_count; i++) {
            for(j = 0; j < feature_count; j++) {

                double nom = (projection_feature_covariance[i] - projection_feature_covariance[j]);
                double denomA = projection_variance; // variance of the projection
                double denomB = covariance_table[i][i] + covariance_table[j][j] - 2*covariance_table[i][j]; // variance of the fi,fj
                // calculate the correlation
                if(denomA == 0.0 || denomB == 0.0) {
                    correlation = 0;
                }
                else {
                    correlation = nom / sqrt(denomA * denomB);
                }
                // printf("corr[%04d][%04d]: %3.2f\n", i,j, correlation);
                if(fabs(correlation) > max_correlation) {
                    uint8_t skip = 0;
                    // check it in the selected features
                    for(p = 0; p < f; p++) {
                        if(selected_features[p][0] == i && selected_features[p][1] == j) {
                            skip = 1;
                            break;
                        }
                        else if(selected_features[p][0] == j && selected_features[p][1] == i) {
                            skip = 1;
                            break;
                        }
                    }
                    if(!skip) {
                        max_correlation = fabs(correlation);
                        // fill the selected features
                        selected_features[f][0] = i;
                        selected_features[f][1] = j;
                    }
                }
            }
        }
        printf("selected[%02d]: %03d - %03d with %3.2f correlation\n", f, selected_features[f][0],selected_features[f][1], max_correlation);
    }

    // clean the alocted memory and return
    free(random_base);
    free(projection);
    free(projection_feature_covariance);

    return SUCCESS;
}

// this function is the original binarizer that proposed in the algorithm
uint32_t fern_default_binarizer(float *data, uint32_t *p, float *t) {
    return (data[p[0]] < data[p[1]]);
}
// this function is the most generic binarizer
uint32_t binarize_two(float *data, uint32_t *p, float *t) {
    float diff = data[p[0]] - data[p[1]];
    return (diff > t[0]) && (diff < t[1]);
}

struct fern_t* fern_default(uint8_t solver) {

    // allocate a fern model for the output
    struct fern_t *model = (struct fern_t*) malloc(sizeof(struct fern_t));
    check_null(model, NULL);

    // set via constructor
    model->solver = solver;
    model->forest_size = 50;
    model->tree_depth = 10;
    model->tree_size = 0;
    model->enable_boosting = 0;
    // how many features will be used for binarization of the feature
    model->nr_feature = 2;

    // sets via fern_set
    model->aux = NULL; // user input for select function

    if(model->solver == FERN_CUSTOM_CLASSIFIER || model->solver == FERN_CUSTOM_REGRESSOR) {
        model->user_function = -1;
    }
    else if(model->solver == FERN_CORRELATION_CLASSIFIER || model->solver == FERN_CORRELATION_REGRESSOR) {
        model->user_function = 0;
        model->select = &fern_correlation_based_feature_selector;
        model->binarize = &fern_default_binarizer;
    }
    else if(model->solver == FERN_RANDOM_CLASSIFIER || model->solver == FERN_RANDOM_REGRESSOR) {
        model->user_function = 0;
        model->select = &fern_random_feature_selector;
        model->binarize = &fern_default_binarizer;
    }
    else {
        model->user_function = -1;
    }

    // allocate and filled in fern_train
    model->is_trained = 0;
    model->nr_class = 0; // number of class
    model->selected_features = NULL;
    model->thresholds = NULL;
    model->weights = NULL;

    return model;
}

struct fern_t* fern_create(uint8_t solver, char options[]) {
    // allocate a fern model for the output
    struct fern_t *model = fern_default(solver);
    // get the options from the user
    getopt(uint32_t, options, "forest_size", &model->forest_size);
    getopt(uint32_t, options, "tree_depth", &model->tree_depth);
    getopt(uint32_t, options, "boost", &model->enable_boosting);

    // set a maximum size for the model tree depth
    if(model->tree_depth > 20) {
        message(WARNING, "tree depth larger than 20 is not supported!");
        model->tree_depth = 20;
    }

    // return the model
    return model;
}

return_t fern_set(struct fern_t *model, return_t (*select)(float*, float*, float*, void**, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t **, float **, uint32_t), uint32_t (*binarize)(float *data, uint32_t *p, float *t), void *aux) {
    // update the feature selection method
    if(select != NULL) {
        model->select = select;
        model->user_function = 1;
    }
    // update the binarization method
    if(binarize != NULL) {
        model->binarize = binarize;
        model->user_function = 1;
    }
    // update the auxiliary pointer
    if(aux != NULL) {
        model->aux = aux;
        model->user_function = 1;
    }
    // return that everything are up to date
    return SUCCESS;
}


// data must be binary for the fern classifier
uint32_t train_fern_regressor(float *data, float *output,
            uint32_t sample_count, uint32_t feature_count, uint32_t output_length,
            struct fern_t *model)
{

    uint32_t p, f,t, forest;

    uint32_t *sample_fern_value = (uint32_t*) malloc(sample_count * sizeof(uint32_t));
    uint32_t *population = (uint32_t*) malloc(model->tree_size * sizeof(uint32_t));
    float *current_output = (float*) calloc(sample_count * feature_count, sizeof(float));

    for(forest = 0; forest < model->forest_size; forest++) {

        // select the features
        // fill: selected_features[forest] and model->thresholds[forest]
        model->select(data, output, current_output, model->aux, sample_count, feature_count, output_length, model->tree_depth, model->selected_features[forest], model->thresholds[forest], forest);

        // set the population of each the tree leaf to zero for the current forest
        memset(population, 0, model->tree_size * sizeof(uint32_t));
        // compute fern index of each person for the selected feature set
        for(p = 0; p < sample_count; p++) {
            float *feature = data + p*feature_count;
            uint32_t fern_value = 0;
            for(f = 0; f < model->tree_depth; f++) {
                fern_value <<= 1;
                // get the binary feature value
                fern_value += model->binarize(feature, model->selected_features[forest][f], model->thresholds[forest][f]);
            }
            sample_fern_value[p] = fern_value;
            float *label = output + p * output_length;
            for(f = 0; f < output_length; f++) {
                model->weights[forest][fern_value][f] += label[f];
            }
            population[fern_value]++;
        }
        // normalize the weights with the number of sample in each tree
        for(t = 0; t < model->tree_size; t++) {
            // normalize the regressor summation with the number of samples in each bin
            for(f = 0; f < output_length; f++) {
                model->weights[forest][t][f] = model->weights[forest][t][f] / (population[t] + 1);
            }
        }
        // find the current output for each sample
        for(p = 0; p < sample_count; p++) {
            float *label = output + p*output_length;
            float *current_sample = current_output + p*output_length;
            // get the whole line of the output
            for(f = 0; f < output_length; f++) {
                current_sample[f] += model->weights[forest][sample_fern_value[p]][f];
            }
            // printf("label: %3.2f current: %3.2f\n", label[0], current_sample[0]);
        }
        // system("pause");
    }
    return 1;
}

uint32_t predict_fern_regressor(matrix_t *in, matrix_t *output, struct fern_t *model) {
    uint32_t p, f, forest;

    // compute fern index of each person for the selected feature set
    for(p = 0; p < rows(in); p++) {
        float *feature = data(float, in, p, 0);
        float *label = data(float, output, p, 0);
        // find the hash value for the given features
        for(forest = 0; forest < model->forest_size; forest++) {
            uint32_t fern_value = 0;
            for(f = 0; f < model->tree_depth; f++) {
                fern_value <<= 1;
                // get the binary feature value
                fern_value += model->binarize(feature, model->selected_features[forest][f], model->thresholds[forest][f]);
            }
            for(f = 0; f < cols(output); f++) {
                label[f] += model->weights[forest][fern_value][f];
            }
        }
    }
    return 1;
}

uint32_t predict_fern_classifier(matrix_t *in, matrix_t *output, struct fern_t *model) {
    uint32_t p, f, forest;

    uint32_t *class_probability = (uint32_t*) malloc(model->nr_class * sizeof(uint32_t));
    // compute fern index of each person for the selected feature set
    for(p = 0; p < rows(in); p++) {
        float *feature = data(float, in, p, 0);
        float *label = data(float, output, p, 0);
        // set the class probability to zero
        memset(label, 0, model->nr_class * sizeof(uint32_t));
        // find the hash value for the given features
        for(forest = 0; forest < model->forest_size; forest++) {
            uint32_t fern_value = 0;
            for(f = 0; f < model->tree_depth; f++) {
                fern_value <<= 1;
                // get the binary feature value
                fern_value += model->binarize(feature, model->selected_features[forest][f], model->thresholds[forest][f]);
            }
            uint32_t class_label = 0;
            for(f = 0; f < cols(output); f++) {
                label[f] += model->weights[forest][fern_value][f];
            }
        }
    }
    return 1;
}

// data must be binary for the fern classifier
uint32_t train_fern_classifier(float *data, float *output,
                          uint32_t sample_count, uint32_t feature_count, uint32_t output_length,
                          struct fern_t *model
) {

    uint32_t p, f,t, forest;

    uint32_t *population = (uint32_t*) malloc(model->tree_size * sizeof(uint32_t));
    uint32_t *sample_fern_value = (uint32_t*) malloc(sample_count * sizeof(uint32_t));
    float *current_output = (float*) calloc(sample_count * feature_count, sizeof(float));

    for(forest = 0; forest < model->forest_size; forest++) {

        // select the features
        // fill: selected_features[forest] and model->thresholds[forest]
        model->select(data, output, current_output, model->aux, sample_count, feature_count, output_length, model->tree_depth, model->selected_features[forest], model->thresholds[forest], forest);

        // set the population of each the tree leaf to zero for the current forest
        memset(population, 0, model->tree_size * sizeof(uint32_t));
        // compute fern index of each person for the selected feature set
        for(p = 0; p < sample_count; p++) {
            float *feature = data + p*feature_count;
            uint32_t fern_value = 0;
            for(f = 0; f < model->tree_depth; f++) {
                fern_value <<= 1;
                // get the binary feature value
                fern_value += model->binarize(feature, model->selected_features[forest][f], model->thresholds[forest][f]);
            }
            sample_fern_value[p] = fern_value;
            float *label = output + p * output_length;
            for(f = 0; f < output_length; f++) {
                model->weights[forest][fern_value][f] += label[f];
            }
            population[fern_value]++;
        }
        // normalize the weights with the number of sample in each tree
        for(t = 0; t < model->tree_size; t++) {
            // normalize the regressor summation with the number of samples in each bin
            for(f = 0; f < output_length; f++) {
                model->weights[forest][t][f] = model->weights[forest][t][f] / (population[t] + 1);
            }
        }

        // find the current output for each sample
        for(p = 0; p < sample_count; p++) {
            float *label = output + p*output_length;
            float *current_sample = current_output + p*output_length;
            // get the whole line of the output
            for(f = 0; f < output_length; f++) {
                current_sample[f] += model->weights[forest][sample_fern_value[p]][f];
            }
            // printf("label: %3.2f current: %3.2f\n", label[0], current_sample[0]);
        }

    }
    return 1;
}

// fern train method
return_t fern_train(matrix_t *input, matrix_t *output, struct fern_t *model) {

    // do regular controls
    if(rows(input) != rows(output)) {
        message(ERROR, "input and output matrices have different number of samples");
        return ERROR_DIMENSION_MISMATCH;
    }
    if(channels(input) != 1) {
        message(WARNING, "expects its arguments to be 1-dimensional matrices");
    }
    if(!is_numeric(input)) {
        message(ERROR, "input must be a numeric matrix");
        return ERROR_TYPE_MISMATCH;
    }

    // start initialization
    uint32_t i = 0, j = 0, c = 0;

    // init variables and allocate memory
    uint32_t sample_count  = rows(input);
    uint32_t feature_count = cols(input);
    uint32_t output_length = cols(output);

    model->nr_class   = cols(output); // assume that we have only one class
    // compute the size of the tree
    model->tree_size = (1u << model->tree_depth);

    // reallocate the memory
    model->selected_features = (uint32_t ***) realloc(model->selected_features, model->forest_size*sizeof(uint32_t**));
    model->thresholds = (float***)  realloc(model->thresholds, model->forest_size*sizeof(float**));
    model->weights = (float***)  realloc(model->weights, model->forest_size*sizeof(float**));

    // set memory to zero
    memset(model->selected_features, 0, model->forest_size * sizeof(uint32_t**));
    memset(model->thresholds, 0, model->forest_size * sizeof(float**));
    memset(model->weights, 0, model->forest_size * sizeof(float**));

    for(i = 0; i < model->forest_size; i++) {
        // reallocate the memory
        model->selected_features[i] = (uint32_t **)  realloc(model->selected_features[i], model->tree_depth*sizeof(uint32_t*));
        model->thresholds[i] = (float **)  realloc(model->thresholds[i], model->tree_depth*sizeof(float*));
        model->weights[i] = (float**)  realloc(model->weights[i], model->tree_size*sizeof(float*));

        // set memory to zero
        memset(model->selected_features[i], 0, model->tree_depth * sizeof(uint32_t*));
        memset(model->thresholds[i], 0, model->tree_depth * sizeof(float*));
        memset(model->weights[i], 0, model->tree_size * sizeof(float*));

        for(c = 0; c < model->tree_depth; c++) {
            // reallocate the memory
            model->selected_features[i][c] = (uint32_t*)  realloc(model->selected_features[i][c], model->nr_feature*sizeof(uint32_t));
            model->thresholds[i][c] = (float*)  realloc(model->thresholds[i][c], model->nr_feature*sizeof(float));
            // set memory to zero
            memset(model->selected_features[i][c], 0, model->nr_feature * sizeof(uint32_t));
            memset(model->thresholds[i][c], 0, model->nr_feature * sizeof(float));
        }
        for(c = 0; c < model->tree_size; c++) {
            model->weights[i][c] = (float*)  realloc(model->weights[i][c], model->nr_class*sizeof(float));
            memset(model->weights[i][c], 0, model->nr_class * sizeof(float));
        }
    }

    // set the auxialry variable to useful computation
    if(model->solver == FERN_CORRELATION_REGRESSOR || model->solver == FERN_CORRELATION_CLASSIFIER) {
        // compute in-feature correlation and fill the correlation table
        float **feature_transpose = (float**) calloc(feature_count, sizeof(float*));
        for(i = 0; i < feature_count; i++) {
            feature_transpose[i] = (float*) calloc(sample_count, sizeof(float));
            // get the feature i of the first sample
            float *feature_data = data(float, input, 0, i);
            for(j = 0; j < sample_count; j++, feature_data += feature_count) {
                feature_transpose[i][j] = feature_data[0];
            }
        }
        // now get space for the correlation table
        float **covariance_table = (float**) calloc(feature_count, sizeof(float*));
        for(i = 0; i < feature_count; i++) {
            covariance_table[i] = (float*) calloc(feature_count, sizeof(float));
            // fill correlation table
            for(j = 0; j < feature_count; j++) {
                covariance_table[i][j] = covariance(feature_transpose[i], feature_transpose[j], sample_count);
                // printf("corr[%04d][%04d]: %3.2f\n", i,j, covariance_table[i][j]);
            }
        }
        // allocate and set the aux
        model->aux = (void**) malloc(2 * sizeof(void*));
        // set the auxilary variables to correlation table
        model->aux[0] = (void*) feature_transpose;
        model->aux[1] = (void*) covariance_table;
    }
    /*
    matrix_t *covImage = matrix_create(uint8_t, 400, 400);
    float **covariance_table = (float**) model->aux[1];
    for(i=0; i < 400; i++) {
        for(j=0; j < 400; j++) {
            at(uint8_t, covImage, i,j) = clamp(2550 * fabs(covariance_table[i][j]), 0, 255);
        }
    }
    imwrite(covImage, "covTable.bmp");
    return ERROR;
    */
    // learn different model for each class (we only support two class/regression)
    if(model->solver == FERN_RANDOM_REGRESSOR || model->solver == FERN_CORRELATION_REGRESSOR || model->solver == FERN_CUSTOM_REGRESSOR) {
        model->is_trained = train_fern_regressor(data(float, input), data(float, output), sample_count, feature_count, output_length, model);
    }
    else if(model->solver == FERN_RANDOM_CLASSIFIER || model->solver == FERN_CORRELATION_CLASSIFIER || model->solver == FERN_CUSTOM_CLASSIFIER) {
        // convert matrix into vector
        model->is_trained = train_fern_classifier(data(float, input), data(float, output), sample_count, feature_count, output_length, model);
    }
    else {
        message(WARNING_NOTHING_DONE, "unknown fern solver");
        model->is_trained = 0;
    }

    // clean auxilary data
    if(model->solver == FERN_CORRELATION_REGRESSOR || model->solver == FERN_CORRELATION_CLASSIFIER) {

        // we dont need the feature transpose, delete it
        float **feature_transpose = (float**) model->aux[0];
        // we dont need the feature transpose, delete it
        for(i = 0; i < feature_count; i++) {
            free(feature_transpose[i]);
        }
        free(feature_transpose);

        // get the address of the aux data
        float **covariance_table = (float**) model->aux[1];
        // we dont need the feature transpose, delete it
        for(i = 0; i < feature_count; i++) {
            free(covariance_table[i]);
        }
        free(covariance_table);

        // now free model aux
        free(model->aux);
        model->aux = NULL;
    }
    // return that the model is trained
    return SUCCESS;
}

// visualize the model onto the given image
matrix_t* fern_visualize(uint32_t out_width, uint32_t out_height, uint32_t scale, struct fern_t *model) {

    // creat a zero initialized output matrix
    matrix_t *out = matrix_create(uint8_t, out_height*scale, out_width*scale, 3);
    // do the usual checks
    check_memory(out, NULL);
    check_null(model, out);
    check_condition(model->is_trained == 1, out, "call fern_train before visualize");

    uint32_t i,j, f, t;

    // make the grid image
    struct color_t gridColor = HSV(200, 10, 100);

    // draw vertical lines
    for(i = 0; i < out_width; i++) {
        draw_line(out, point(i*scale,0,0), point(i*scale, scale*out_height-1,0), gridColor, 1);
    }
    draw_line(out, point(i*scale-1,0,0), point(i*scale-1, scale*out_height-1,0), gridColor, 1);
    // draw horizontal lines
    for(j = 0; j < out_height; j++) {
        draw_line(out, point(0,j*scale,0), point(scale*out_width-1,j*scale,0), gridColor, 1);
    }
    draw_line(out, point(0,j*scale-1,0), point(scale*out_width-1,j*scale-1,0), gridColor, 1);

    if(model->selected_features == NULL) {
        return out;
    }

    for(f = 0; f < model->forest_size; f++) {

        uint8_t h = random_int(0,255);
        uint8_t s = random_int(200,255);
        uint8_t v = random_int(100,255);

        struct color_t strokeColor = HSV(h,s,v);

        int offsetX = random_int(1, scale-2);
        int offsetY = random_int(1, scale-2);

        for(t = 0; t < model->tree_depth; t++) {

            float p1x = (float) (model->selected_features[f][t][0] % out_width) * scale + offsetX;
            float p1y = ceilf(model->selected_features[f][t][0] / out_width)  * scale + offsetY;

            float p2x = (float) (model->selected_features[f][t][1] % out_width)  * scale + offsetX;
            float p2y =  ceilf(model->selected_features[f][t][1] / out_width)  * scale + offsetY;
            // printf("pointA: %3.2f %3.2f \t pointB: %3.2f %3.2f", p1x, p1y, p2x, p2y);
            draw_line(out, point(p1x,p1y,0), point(p2x,p2y,0), strokeColor, 2);
        }

    }

    return out;
}

return_t fern_predict(matrix_t *in, matrix_t *output, struct fern_t *model) {

    // check that the given model is valid
    check_null(model, WARNING_NOTHING_DONE);
    check_condition(model->is_trained == 1, ERROR_TYPE_MISMATCH, "call fern_train before predict");
    check_condition(model->user_function != -1, ERROR_TYPE_MISMATCH, "call fern_set before predict");

    if(model->solver == 0) {
        predict_fern_regressor(in, output, model);
    }
    else {
        predict_fern_classifier(in, output, model);
    }

    return SUCCESS;
}

return_t fern_write(struct fern_t *model, const char *filename) {

    uint32_t i, c;
    // create the file with the given key value pair
    FILE *out = imlab_fopen(filename, "wb", "fern");
    check_file(out, ERROR_UNABLE_TO_OPEN);

    // read the model
    fwrite(&model->solver, sizeof(uint8_t), 1, out);
    fwrite(&model->forest_size, sizeof(uint32_t), 1, out);
    fwrite(&model->tree_depth, sizeof(uint32_t), 1, out);
    fwrite(&model->tree_size, sizeof(uint32_t), 1, out);
    fwrite(&model->nr_feature, sizeof(uint32_t), 1, out);
    fwrite(&model->enable_boosting, sizeof(uint32_t), 1, out);

    // write that user set any function or not
    fwrite(&model->user_function, sizeof(int8_t), 1, out);
    // if user set any function ask him to set them again
    if(model->user_function == 1) {
        message(WARNING, "cannot write custom functions, please call fern_set after reading the model");
    }

    // write if the training is done
    fwrite(&model->is_trained, sizeof(uint32_t), 1, out);
    fwrite(&model->nr_class, sizeof(uint32_t), 1, out);
    // save the values if the model is trained
    if(model->is_trained > 0) {

        for(i = 0; i < model->forest_size; i++) {
            for(c = 0; c < model->tree_depth; c++) {
                fwrite(model->selected_features[i][c], sizeof(uint32_t), model->nr_feature, out);
                fwrite(model->thresholds[i][c], sizeof(float), model->nr_feature, out);
            }
            for(c = 0; c < model->tree_size; c++) {
                fwrite(model->weights[i][c], sizeof(float), model->nr_class, out);
            }
        }
    }
    // we cannot write the aux
    fclose(out);

    return SUCCESS;
}

// try to read data from the source, print error message and return on failure
#define safe_read(_value, _size, _count, _src, _ret) do {\
    if(fread((_value), (_size), (_count), (_src)) <= 0)\
    {\
        message(ERROR_UNABLE_TO_READ, NULL);\
        return _ret;\
    }\
} while(0);

struct fern_t* fern_read(const char *filename) {

    uint32_t i, c;
    // open the file with the given key value pair
    FILE *fin = imlab_fopen(filename, "rb", "fern");
    check_file(fin, NULL);

    // create an empty fern container
    struct fern_t *model = (struct fern_t*) calloc(1, sizeof(struct fern_t));

    // read the model
    safe_read(&model->solver, sizeof(uint8_t), 1, fin, model);
    safe_read(&model->forest_size, sizeof(uint32_t), 1, fin, model);
    safe_read(&model->tree_depth, sizeof(uint32_t), 1, fin, model);
    safe_read(&model->tree_size, sizeof(uint32_t), 1, fin, model);
    safe_read(&model->nr_feature, sizeof(uint32_t), 1, fin, model);
    safe_read(&model->enable_boosting, sizeof(uint32_t), 1, fin, model);

    // write that user set any function or not
    safe_read(&model->user_function, sizeof(int8_t), 1, fin, model);

    // set the appropriate functions
    if(model->user_function == 1 || model->solver == FERN_CUSTOM_CLASSIFIER || model->solver == FERN_CUSTOM_REGRESSOR) {
        message(WARNING, "fern model is constructed via custom functions, please call fern_set before fern_train or fern_predict");
        model->user_function = -1;
    }
    else if(model->solver == FERN_CORRELATION_CLASSIFIER || model->solver == FERN_CORRELATION_REGRESSOR) {
        model->user_function = 0;
        model->select = &fern_correlation_based_feature_selector;
        model->binarize = &fern_default_binarizer;
    }
    else if(model->solver == FERN_RANDOM_CLASSIFIER || model->solver == FERN_RANDOM_REGRESSOR) {
        model->user_function = 0;
        model->select = &fern_random_feature_selector;
        model->binarize = &fern_default_binarizer;
    }
    else {
        model->user_function = -1;
    }

    model->aux = NULL;
    // write if the training is done
    safe_read(&model->is_trained, sizeof(uint32_t), 1, fin, model);
    safe_read(&model->nr_class, sizeof(uint32_t), 1, fin, model);
    // allocate space and read the learned coefficieints if the training is done
    if(model->is_trained > 0) {
        // allocate space for the output
        model->selected_features = (uint32_t ***) calloc(model->forest_size,sizeof(uint32_t**));
        model->thresholds = (float***)  calloc(model->forest_size,sizeof(float**));
        model->weights = (float***)  calloc(model->forest_size,sizeof(float**));

        for(i = 0; i < model->forest_size; i++) {
            // allocate space for the output
            model->selected_features[i] = (uint32_t **)  calloc(model->tree_depth,sizeof(uint32_t*));
            model->thresholds[i] = (float **)  calloc(model->tree_depth,sizeof(float*));
            model->weights[i] = (float**)  calloc(model->tree_size,sizeof(float*));

            for(c = 0; c < model->tree_depth; c++) {
                // allocate space for the output
                model->selected_features[i][c] = (uint32_t*)  calloc(model->nr_feature,sizeof(uint32_t));
                model->thresholds[i][c] = (float*)  calloc(model->nr_feature,sizeof(float));
                // read the written values
                safe_read(model->selected_features[i][c], sizeof(uint32_t), model->nr_feature, fin, model);
                safe_read(model->thresholds[i][c], sizeof(float), model->nr_feature, fin, model);
            }
            for(c = 0; c < model->tree_size; c++) {
                model->weights[i][c] = (float*)  calloc(model->nr_class,sizeof(float));
                // read the written values
                safe_read(model->weights[i][c], sizeof(float), model->nr_class, fin, model);
            }
        }
    }
    else {
        model->selected_features = NULL;
        model->thresholds = NULL;
        model->weights = NULL;
    }

    // return the construted model
    return model;
}