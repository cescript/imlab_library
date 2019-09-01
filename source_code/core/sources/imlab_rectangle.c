/**
\brief Functions for the matrix_t containers
- rectangle_create()  : constructor function zero initializer
*/
#include <stdlib.h>
#include "private/core_private.h"
#include "private/alcore_private.h"

/*
struct rectangle_t {
  int32_t x, y, width, height;
  float coefficient;
};
 */
struct rectangle_t rectangle(int32_t x, int32_t y, int32_t width, int32_t height, float coefficient) {
    struct rectangle_t rect;
    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
    rect.coefficient = coefficient;
    // return the created rectangle
    return rect;
};
// mode 0: return the overlapping area between the two rectangle
// mode 1: normalze the overlapping area with respect to the input rectangles area
float rectangle_overlap(struct rectangle_t r1, struct rectangle_t r2, uint8_t mode) {
    //
    float left  = max(r1.x, r2.x);
    float right = min(r1.x + r1.width, r2.x + r2.width);

    float top    = max(r1.y, r2.y);
    float bottom = min(r1.y + r1.height, r2.y + r2.height);

    // compute the intersection area
    float intersection_area = max(0, (bottom - top)) * max(0, (right - left));

    float result = 0;
    // if the mode is zero, return the intersecting area
    if(mode == 0) {
        result = intersection_area;
    }
    // otherwise return the normalized intersecting area
    else if(mode == 1) {
        float a1 = r1.width * r1.height;
        float a2 = r2.width * r2.height;
        float union_area = a1 + a2 - intersection_area;
        // compute the normalized area
        result = intersection_area / union_area;
    }

    // return the overlapping area between the rectangles
    return result;
}

static int rectangle_compare(const void *pa, const void *pb)
{
    struct rectangle_t *a = (struct rectangle_t *)pa;
    struct rectangle_t *b = (struct rectangle_t *)pb;
    // compute the difference and return +-1 or 0
    float diff = a->coefficient - b->coefficient;

    if (diff < 0) return 1;
    else if (diff > 0) return -1;
    return 0;
}

// merge multiple rectangle into the new one
vector_t *rectangle_merge(vector_t *rect, float overlap_threshold, uint32_t min_groups_size)
{
    uint32_t i, j, l;
    // create a disjoint set data structure
    struct disjoint_set_t *rect_set = disjoint_set_create(length(rect));
    // get the pointer to the data
    struct rectangle_t *rect_data = data(struct rectangle_t, rect);

    // go on every rectangle and connect overlapping ones
    for(i = 0; i < length(rect); i++) {
        for(j = 0; j < length(rect); j++) {

            if(i == j) {
                continue;
            }
            // get the overlapping area between the two rectangle
            float overlap = rectangle_overlap(rect_data[i], rect_data[j], 1);
            // connect if the overlapping area is larger than the threshold value
            if (overlap > overlap_threshold)
            {
                disjoint_set_union(rect_set, i, j);
            }
        }
    }
    // call enumerate and assign unique labels to all elements
    uint32_t number_of_connected_components = disjoint_set_enumerate(rect_set);

    // create output vector for merged rectangles
    vector_t *merged = vector_create(struct rectangle_t);

    // get the mean of the overlapping rectangles
    for (l = 0; l < number_of_connected_components; l++)
    {
        uint32_t element_count = 0;
        struct rectangle_t rectI = {0};

        // check all the rectangles and sum the ones labeled as label
        for (i = 0; i < length(rect); i++)
        {
            // if the label of the current rectangle matches l
            if (l == rect_set->label[i])
            {
                element_count++;

                rectI.x += rect_data[i].x;
                rectI.y += rect_data[i].y;
                rectI.width += rect_data[i].width;
                rectI.height += rect_data[i].height;
                rectI.coefficient += rect_data[i].coefficient;
            }
        }

        if (element_count > min_groups_size)
        {
            // normalize the summation
            rectI.x /= element_count;
            rectI.y /= element_count;
            rectI.width /= element_count;
            rectI.height /= element_count;
            rectI.coefficient /= element_count;

            // add the group to the output set
            vector_push(merged, &rectI);
        }
    }

    // free the disjoint set structure
    disjoint_set_free(&rect_set);

    // return the merged rectangle
    return merged;
};
