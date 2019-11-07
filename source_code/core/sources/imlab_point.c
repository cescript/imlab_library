#include <stdlib.h>
#include <math.h>
#include "private/core_private.h"
#include "private/alcore_private.h"

struct point_t point(float x, float y, float z) 
{
    struct point_t pt;
    pt.x = x;
    pt.y = y;
    pt.z = z;
    // return the created point
    return pt;
};

float point_distance(struct point_t p1, struct point_t p2) 
{
    double dx = (double)(p1.x - p2.x);
    double dy = (double)(p1.y - p2.y);
    double dz = (double)(p1.z - p2.z);
    // return the distance between the points
    return sqrtf(dx*dx + dy*dy + dz*dz);
}

// fit rectangle to the input points
// Implementation of the paper Finding best-ﬁtted rectangle for regions using a bisection method
// Returns the four vertices (TopLeft, TopRight, BottomLeft, BottomRight) of the minimum area recctangle fitted to the input points.
return_t point_fit_rectangle(struct point_t *input, uint32_t input_length, struct point_t output[4])
{
    int cond1 = input_length > 0;
    check_condition(cond1, ERROR_DIMENSION_MISMATCH, "input length must be greater than zero");

    // check that the input pointer is filled and allocated
    check_null(input, ERROR_NULL_TYPE);

    double Sxx = 0;
    double Sx2 = 0;
    double Syy = 0;
    double Sy2 = 0;
    double Sxy = 0;

    // do second pass by resolving the labels and filling min and max
    uint32_t l;
    for (l = 0; l < input_length; l++)
    {
        Sxx += input[l].x;
        Syy += input[l].y;
        Sx2 += input[l].x * input[l].x;
        Sy2 += input[l].y * input[l].y;
        Sxy += input[l].x * input[l].y;
    }

    float Ex1 = Sxx / (double)input_length;
    float Ey1 = Syy / (double)input_length;
    float Ex2 = Sx2 / (double)input_length;
    float Ey2 = Sy2 / (double)input_length;
    float Exy = Sxy / (double)input_length;

    // compute the covariance matrix elements
    float Cxx = Ex2 - Ex1 * Ex1;
    float Cxy = Exy - Ex1 * Ey1;
    float Cyy = Ey2 - Ey1 * Ey1;

    // fill center and major axis orienttaion
    float Theta = atan2(2 * Cxy, Cxx - Cyy) / 2.0f;
    float TanTheta = tan(Theta);
    float CotTheta = TanTheta != 0.0f ? 1.0f / TanTheta : 0;

    // allocate min and max vlues
    uint32_t x[4];
    uint32_t y[4];

    float posMajorMin = 0;
    float posMajorMax = 0;
    float posMinorMin = 0;
    float posMinorMax = 0;

    for (l = 0; l < input_length; l++)
    {
        // which side is the point wrt major axis
        float posMajor = (input[l].y - Ey1) - TanTheta * (input[l].x - Ex1);
        float posMinor = (input[l].y - Ey1) + CotTheta * (input[l].x - Ex1);

        // determine the upper and lower points wrt to the minor axis
        if (posMajor > posMajorMax)
        {
            posMajorMax = posMajor;
            x[0] = input[l].x;
            y[0] = input[l].y;
        }
        else if (posMajor < posMajorMin)
        {
            posMajorMin = posMajor;
            x[1] = input[l].x;
            y[1] = input[l].y;
        }

        // determine the upper and lower points wrt to the minor axis
        if (posMinor > posMinorMax)
        {
            posMinorMax = posMinor;
            x[2] = input[l].x;
            y[2] = input[l].y;
        }
        else if (posMinor < posMinorMin)
        {
            posMinorMin = posMinor;
            x[3] = input[l].x;
            y[3] = input[l].y;
        }
    }

    float div = TanTheta + CotTheta;
    output[0].x = (x[0] * TanTheta + x[2] * CotTheta + y[2] - y[0]) / div;
    output[0].y = (y[0] * CotTheta + y[2] * TanTheta + x[2] - x[0]) / div;

    output[1].x = (x[0] * TanTheta + x[3] * CotTheta + y[3] - y[0]) / div;
    output[1].y = (y[0] * CotTheta + y[3] * TanTheta + x[3] - x[0]) / div;

    output[2].x = (x[1] * TanTheta + x[2] * CotTheta + y[2] - y[1]) / div;
    output[2].y = (y[1] * CotTheta + y[2] * TanTheta + x[2] - x[1]) / div;

    output[3].x = (x[1] * TanTheta + x[3] * CotTheta + y[3] - y[1]) / div;
    output[3].y = (y[1] * CotTheta + y[3] * TanTheta + x[3] - x[1]) / div;

    return SUCCESS;
}

// merge multiple points into the new one
vector_t *point_merge(vector_t *points, float overlap_distance, uint32_t min_groups_size)
{
    uint32_t i, j, l;
    // create a disjoint set data structure
    struct disjoint_set_t *point_set = disjoint_set_create(length(points));
    // get the pointer to the data
    struct point_t *point_data = data(struct point_t, points);

    // go on every rectangle and connect overlapping ones
    for (i = 0; i < length(points); i++)
    {
        for (j = 0; j < length(points); j++)
        {

            if (i == j)
            {
                continue;
            }
            // get the overlapping area between the two rectangle
            float distance = point_distance(point_data[i], point_data[j]);

            // connect if the overlapping area is larger than the threshold value
            if (distance < overlap_distance)
            {
                disjoint_set_union(point_set, i, j);
            }
        }
    }
    // call enumerate and assign unique labels to all elements
    uint32_t number_of_connected_components = disjoint_set_enumerate(point_set);

    // create output vector for merged points
    vector_t *merged = vector_create(struct point_t);

    // get the mean of the overlapping rectangles
    for (l = 0; l < number_of_connected_components; l++)
    {
        uint32_t element_count = 0;
        struct point_t pointClusterCenter = {0};

        // check all the points and sum the ones labeled as label
        for (i = 0; i < length(points); i++)
        {
            // if the label of the current point matches l
            if (l == point_set->label[i])
            {
                element_count++;

                pointClusterCenter.x += point_data[i].x;
                pointClusterCenter.y += point_data[i].y;
                pointClusterCenter.z += point_data[i].z;
            }
        }

        if (element_count > min_groups_size)
        {
            // normalize the summation
            pointClusterCenter.x /= element_count;
            pointClusterCenter.y /= element_count;
            pointClusterCenter.z /= element_count;

            // add the group to the output set
            vector_push(merged, &pointClusterCenter);
        }
    }

    // free the disjoint set structure
    disjoint_set_free(&point_set);

    // return the merged points
    return merged;
};