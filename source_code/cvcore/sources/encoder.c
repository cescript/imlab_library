#include <stdlib.h>
#include "private/iocore_private.h"
#include "private/cvcore_private.h"


int encoder_parameters(uint32_t width, uint32_t height, uint32_t channels, char *options, struct encoder_parameters_t *out) 
{
    out->type = 0;

    if(out->type == 0) 
    {
        out->feature_size = width * height * channels;
    } 
    else 
    {
        out->feature_size = 0;
    }

    return out->feature_size;
}

// create an encoder object inside the feature_t pointer
struct feature_t *encoder_create(uint32_t width, uint32_t height, uint32_t channels, char options[]) {

    // create a feature model for the output
    struct feature_t *model = (struct feature_t*) malloc(sizeof(struct feature_t));
    check_memory(model, NULL);
    // fill the algorithm name
    model->algorithm = CV_ENCODER;
    model->image_width = width;
    model->image_height = height;
    model->image_channels = channels;

    // get space for the parameters of the algorithm
    model->parameters = malloc(sizeof(struct encoder_parameters_t));
    check_memory(model->parameters) 
    {
        free(model);
        return NULL;
    }
    // fill the parameters
    model->feature_size = encoder_parameters(width, height, channels, options, model->parameters);

    if(model->feature_size == 0) 
    {
        free(model->parameters);
        free(model);
        return NULL;
    }

    // get the default feature extraction method
    model->method = encoder_extract;

    // return the created model
    return model;
}

return_t encoder_extract(matrix_t *in, struct feature_t *par_model, float *feature) 
{
    // get the Encoder parameters from the given model
    struct encoder_parameters_t *model = par_model->parameters;

    if(model->type == 0) 
    {
        return imencode_gray(in, feature);
    }
    else 
    {
        return WARNING_NOT_SUPPORTED;
    }
}

return_t imencode_gray(matrix_t *in, float *feature) 
{
    uint32_t idx;

    uint8_t *in_data = data(uint8_t, in);

    if(channels(in) == 1) 
    {
        for(idx = 0; idx < width(in) * height(in); idx++) 
        {
            feature[idx] = (float)(in_data[idx]) / 255.0f;
        }
    } 
    else if(channels(in) == 3) 
    {
        for(idx = 0; idx < width(in) * height(in); idx++) 
        {
            feature[idx] = (0.3f * in_data[3*idx + 2] + 0.6f * in_data[3*idx + 1] + 0.1f * in_data[3*idx + 0]) / 255.0f;
        }
    }
    return SUCCESS;
}

matrix_t *encoder2image(float *feature, struct feature_t *par_model)
{
    // get the encoder parameters
    struct encoder_parameters_t *model = par_model->parameters;

    // create an output image
    matrix_t *image = matrix_create(uint8_t, par_model->image_height, par_model->image_width, 3);

    // check that the necessary memories are allocated
    check_null(image, matrix_null());
    
    uint8_t *img_pointer = mdata(image, 0);

    uint32_t idx_gray = 0, idx = 0;
    for (idx_gray = 0; idx_gray < par_model->image_height * par_model->image_width; idx_gray++, idx += 3)
    {
        img_pointer[idx + 0] = feature[idx_gray] * 255;
        img_pointer[idx + 1] = feature[idx_gray] * 255;
        img_pointer[idx + 2] = feature[idx_gray] * 255;
    }

    return image;
}