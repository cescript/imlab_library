#include <stdlib.h>
#include "private/iocore_private.h"
#include "private/cvcore_private.h"

int npd_parameters(uint32_t width, uint32_t height, uint32_t channels, char *options, struct npd_parameters_t *out) 
{
    out->min_distance = 0;
    out->max_distance = max(width,height);
    out->n_sample = 1000;

    // get the user given parameters
    getopt(uint32_t, options, "min_distance", &out->min_distance);
    getopt(uint32_t, options, "max_distance", &out->max_distance);
    getopt(uint32_t, options, "n_sample", &out->n_sample);

     // set the feature size
    out->feature_size = out->n_sample;

    // allocate the memory
    out->selected[0] = (uint32_t*) malloc(out->feature_size * sizeof(uint32_t));
    out->selected[1] = (uint32_t*) malloc(out->feature_size * sizeof(uint32_t));

    // before using rand() make sure that the input is reproducable
    srand(0);

    uint32_t i = 0;
    for(i = 0; i < out->feature_size; i++)
    {
        uint32_t c1 = rand() % channels;
        uint32_t c2 = rand() % channels;

        // find random first point
        uint32_t x1 = rand() % width;
        uint32_t y1 = rand() % height;

        uint32_t x2 = rand() % width;
        uint32_t y2 = rand() % height;

        out->selected[0][i] = c1 + channels * (x1 + width * y1);
        out->selected[1][i] = c2 + channels * (x2 + width * y2);
    }

    return out->feature_size;
}

void npd_view(struct feature_t *par_model) 
{
    struct npd_parameters_t *model = par_model->parameters;

    printf("Parameters of the Normalized Pixel Difference Model\n");
    printf("Options:\n");
    printf("> Mininum Distance    : %d\n", model->min_distance);
    printf("> Maximum Distance    : %d\n", model->max_distance);
    printf("> Number of Samples   : %d\n", model->n_sample);

    printf("Computed Values:\n");
    printf("> Feature Size        : %d\n", model->feature_size);
}

// create an npd object inside the feature_t pointer
struct feature_t *npd_create(uint32_t width, uint32_t height, uint32_t channels, char options[]) 
{
    // create a feature model for the output
    struct feature_t *model = (struct feature_t*) malloc(sizeof(struct feature_t));
    check_memory(model, NULL);

    // fill the algorithm name
    model->algorithm = CV_NPD;
    model->image_width = width;
    model->image_height = height;
    model->image_channels = channels;

    // get space for the parameters of the algorithm
    model->parameters = malloc(sizeof(struct npd_parameters_t));
    check_memory(model->parameters) 
    {
        free(model);
        return NULL;
    }
    // fill the parameters
    model->feature_size = npd_parameters(width, height, channels, options, model->parameters);

    if(model->feature_size == 0) 
    {
        free(model->parameters);
        free(model);
        return NULL;
    }
    // get the default feature extraction method
    model->method = npd_extract;

    // return the created model
    return model;
}

return_t npd_extract(matrix_t *in, struct feature_t *par_model, float *feature) 
{
    // get the npd parameters from the given model
    struct npd_parameters_t *model = par_model->parameters;

    // get the data pointer
    uint8_t *in_data = data(uint8_t, in);

    uint32_t f;
    for(f = 0; f < model->feature_size; f++) 
    {
        float x = in_data[model->selected[0][f]];
        float y = in_data[model->selected[1][f]];

        // initialize the feature
        feature[f] = 0.0f;

        // if the denominator > 0, compude NPD
        if(x != 0.0f || y != 0.0f)
        {
            feature[f] = (x - y) / (x + y);
        }
    }

    return SUCCESS;
}
