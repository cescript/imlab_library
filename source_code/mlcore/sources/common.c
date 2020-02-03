// common functions
#include "private/mlcore_private.h"

// logistic function and its derivatives
float sigmoid(float x)
{
    return 1 / (1 + expf(-x));
}
float sigmoid_d(float x)
{
    float sx = sigmoid(x);
    return sx * (1 - sx);
}

// tangent hyperbolic function and its derivative
float tanhyper(float x)
{
    return tanh(x);
}
float tanhyper_d(float x)
{
    float sx = tanh(x);
    return 1 - sx * sx;
}

// relu function and its derivative
float relu(float x)
{
    return x > 0 ? x : 0;
}
float relu_d(float x)
{
    return x > 0 ? 1 : 0;
}
