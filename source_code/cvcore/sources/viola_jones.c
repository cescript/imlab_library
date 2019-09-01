#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "private/cvcore_private.h" // for color_t and HSV

struct haar_feature_t *haar_feature_create(int tilt, int length)
{
    struct haar_feature_t *out = (struct haar_feature_t *)malloc(sizeof(struct haar_feature_t));
    // copy the input paramaters
   
    out->tilt = tilt;

    out->length = length;
    out->rects = (struct rectangle_t *)calloc(length, sizeof(struct rectangle_t));

    return out;
}

struct haar_tree_t *haar_tree_create(double thr, double lv, double rv)
{
    struct haar_tree_t *out = (struct haar_tree_t *)malloc(sizeof(struct haar_tree_t));

    out->threshold = thr;
    out->l_val = lv;
    out->r_val = rv;

    out->feats = (struct haar_feature_t *)calloc(1, sizeof(struct haar_feature_t));

    return out;
}

struct haar_stage_t *haar_stage_create(double thr, int parent, int next, int length)
{
    struct haar_stage_t *out = (struct haar_stage_t *)malloc(sizeof(struct haar_stage_t));

    out->stage_threshold = thr;
    out->parent = parent;
    out->next = next;

    out->length = length;
    out->trees = (struct haar_tree_t **)calloc(length, sizeof(struct haar_tree_t *));

    return out;
}

struct haar_t *haar_create(int s1, int s2, int length)
{
    struct haar_t *out = (struct haar_t *)malloc(sizeof(struct haar_t));

    out->size1 = s1;
    out->size2 = s2;
    out->length = length;
    out->stages = (struct haar_stage_t **)calloc(length, sizeof(struct haar_stage_t *));

    return out;
}

struct haar_t *haar_read(const char *filename)
{

    uint32_t i, j, k, l;
    // extension must be JSON
    vector_t *root = json_read(filename);

    int feature_width = json_get_number(root, "feature_width", 0);
    int feature_height = json_get_number(root, "feature_height", 0);

    vector_t *stages = json_get_array(root, "stages", 0);

    // width, height and length of the stages
    struct haar_t *out = haar_create(feature_width, feature_height, length(stages));

    // each haar classifier consists of different number of stages
    for (i = 0; i < length(stages); i++)
    {
        // each stage has a stage threshold and a tree array
        vector_t *stage_i = json_get_object(stages, i);

        double stage_threshold = json_get_number(stage_i, "stage_threshold", 0);
        int parent = json_get_number(stage_i, "parent", 0);
        int next = json_get_number(stage_i, "next", 0);

        // get the trees array under the stage i
        vector_t *trees = json_get_array(stage_i, "trees", 0);

        // create a stage variable for the current classifier
        out->stages[i] = haar_stage_create(stage_threshold, parent, next, length(trees));

        // each tree has different number of feature arrays
        for (j = 0; j < length(trees); j++)
        {
            vector_t *trees_j = json_get_object(trees, j);

            // each tree has number of paramaters for the algorithm
            double tr = json_get_number(trees_j, "threshold", 0);
            double lv = json_get_number(trees_j, "left_val", 0);
            double rv = json_get_number(trees_j, "right_val", 0);

            // get the features
            // a tree only has a feature array in it
            vector_t *feats = json_get_object(trees_j, "feature", 0);

            out->stages[i]->trees[j] = haar_tree_create(tr, lv, rv);

            // get feature elements
            int tt = atoi(json_get_string(feats, "tilted", 0));
            vector_t *rects = json_get_array(feats, "rects", 0);

            out->stages[i]->trees[j]->feats = haar_feature_create(tt, length(rects));

            int x = 0, y = 0, w = 0, h = 0;
            float c = 0;

            for (l = 0; l < length(rects); l++)
            {
                char *rect_l = json_get_string(rects, l);
                sscanf(rect_l, "%d %d %d %d %f", &x, &y, &w, &h, &c);

                // create rectangle
                out->stages[i]->trees[j]->feats->rects[l] = rectangle(x, y, w, h, c);

            } // end of rectangles in feats k
        }     // end of trees in stage i
    }         // end of stages

    vector_free(&root);

    return out;
}

vector_t* haar_detector(matrix_t *img, struct haar_t *cascade, float minSize, float maxSize, float scaleFactor, float stepSize)
{
    int modelWindowWidth = cascade->size1;
    int modelWindowHeight = cascade->size2;

    // input params
    float minSizeScale = minSize / modelWindowWidth;
    float maxSizeScale = maxSize / modelWindowHeight;

    // output
    vector_t *detections = vector_create(struct rectangle_t);

    // necessary variables
    int i, j, s, t, f, r;
    float scale, sum_stage, sum_feature;

    matrix_t *sums = matrix_create(float);
    matrix_t *ssum = matrix_create(float);

    // compute the integral image
    integral(img, sums, ssum);

    // start haar classifier for different scales
    for (scale = minSizeScale; scale < maxSizeScale; scale *= scaleFactor)
    {
        // WS
        int currentWindowWidth = modelWindowWidth * scale; 
        int currentWindowHeight = modelWindowHeight * scale;

        if (!(currentWindowWidth < cols(img) && currentWindowHeight < rows(img)))
        {
            break;
        }

        float InvArea = 1.0f / (currentWindowWidth * currentWindowHeight);
        int stride = round(scale * stepSize);

        for (j = 0; j < rows(img) - currentWindowHeight; j += max(1,stride))
        {
            for (i = 0; i < cols(img) - currentWindowWidth; i += max(1, stride))
            {
                // get variance and mean

                float e2 = InvArea * integral_get_float(ssum, i, j, i + currentWindowWidth - 1, j + currentWindowHeight - 1, 0);
                float e1 = InvArea * integral_get_float(sums, i, j, i + currentWindowWidth - 1, j + currentWindowHeight - 1, 0);

                float variance = (e2 - e1 * e1);

                // find the variance factor
                float stdev = sqrt(variance > 0 ? variance:1);

                // check for each stage
                for (s = 0; s < cascade->length; s++)
                {
                    double sum_stage = 0;

                    // each stage has a stage threshold and a tree array
                    struct haar_stage_t *stage_s = cascade->stages[s];

                    // check a stage pass the threshold
                    for (t = 0; t < stage_s->length; t++)
                    {
                        // each tree has number of paramaters for the algorithm
                        struct haar_tree_t *trees_t = stage_s->trees[t];

                        // examine each feature
                        float sum_feature = 0;
                        for (r = 0; r < trees_t->feats->length; r++)
                        {                            
                            int x1 = round(i + scale * trees_t->feats->rects[r].x);
                            int y1 = round(j + scale * trees_t->feats->rects[r].y);

                            int w = round(scale * trees_t->feats->rects[r].width);
                            int h = round(scale * trees_t->feats->rects[r].height);

                            sum_feature += integral_get_float(sums, x1, y1, x1 + w - 1, y1 + h - 1, 0) * trees_t->feats->rects[r].coefficient;
                        }

                        // choose left or right node depending on the feature summation
                        sum_stage += (sum_feature * InvArea < stdev * trees_t->threshold) ? trees_t->l_val : trees_t->r_val;
                    }
                    
                    // check that the sum is larger than the stage threshold
                    if (sum_stage < stage_s->stage_threshold)
                    {
                        break;
                    }
                }
                // push the detected object into the output buffer
                if (s  == cascade->length)
                {
                    struct rectangle_t obj = rectangle(i, j, currentWindowWidth, currentWindowHeight, 1.0);
                    vector_push(detections, &obj);
                }
            }
        }
    }

    // clear the allocations
    matrix_free(&sums);
    matrix_free(&ssum);

    // return the detections
    return detections;
}