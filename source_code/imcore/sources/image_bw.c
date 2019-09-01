#include <stdlib.h>
#include "private/core_private.h"
#include "private/imcore_private.h"

return_t imerode(matrix_t *in, matrix_t *element, matrix_t *out) {

    check_image(in , ERROR_NOT_IMAGE);
    check_image(out, ERROR_NOT_IMAGE);
    // check_numeric(element, ERROR_TYPE_MISMATCH);

    int cond1 = is_image(in) & is_image(out) & is_32f(element);
    check_condition(cond1, ERROR_TYPE_MISMATCH, "input and output must be uint8 and element must be float arrays");

    int cond2 = (channels(in) == 1) & (channels(out) == 1) & (channels(element) == 1);
    check_condition(cond2, ERROR_DIMENSION_MISMATCH, "input/output and element must be 2D arrays");

    uint32_t w,h, s;

    // TODO: create in_data pointer based on the input type
    uint8_t *in_data  = data(uint8_t, in);
    uint8_t *out_data = data(uint8_t, out);

    // allocate memory for the structural element and element inices
    int *str_elem = (int*) malloc(rows(element)*cols(element)*sizeof(int));
    int *str_idxs = (int*) malloc(rows(element)*cols(element)*sizeof(int));
    // find center of the structral element
    int cw  = (cols(element)-1)/2;
    int ch  = (rows(element)-1)/2;

    float *elem_data = data(float, element);
    // Make structral element as 1d array
    for(h = 0; h < rows(element); h++) {
        for(w = 0; w < cols(element); w++) {
            s = w + cols(element)*h;
            str_elem[s] = (elem_data[w+h*cols(element)] > 0.5);
            str_idxs[s] = (w-cw) + (h- ch)*width(in);
        }
    }

    unsigned char hit;
    #pragma omp parallel for private(h, w, s, hit)
    for(h = ch; h < height(in)-ch; h++) {
        for(w = cw; w < width(in)-cw; w++) {
        // main loop over the image
            hit = 255;
            int idx = w+width(in)*h;
            for(s = 0; s < rows(element)*cols(element); s++) {
                 if ( !(str_elem[s] && in_data[idx + str_idxs[s]]) ) { hit = 0; break; }
            }
            out_data[idx] = hit;
        // compute fit and hit
        }
    }

    return SUCCESS;
}

return_t imdilate(matrix_t *in, matrix_t *element, matrix_t *out) {

    check_image(in , ERROR_NOT_IMAGE);
    check_image(out, ERROR_NOT_IMAGE);

    int cond1 = is_image(in) & is_image(out) & is_32f(element);
    check_condition(cond1, ERROR_TYPE_MISMATCH, "input and output must be uint8 and element must be float arrays");

    int cond2 = (channels(in) == 1) & (channels(out) == 1) & (channels(element) == 1);
    check_condition(cond2, ERROR_DIMENSION_MISMATCH, "input/output and element must be 2D arrays");

    int w,h, s;

    // TODO: create in_data pointer based on the input type
    uint8_t *in_data  = data(uint8_t, in);
    uint8_t *out_data = data(uint8_t, out);

    // allocate memory for the structural element and element inices
    int *str_elem = (int*) malloc(rows(element)*cols(element)*sizeof(int));
    int *str_idxs = (int*) malloc(rows(element)*cols(element)*sizeof(int));
    // find center of the structral element
    int cw  = (cols(element)-1)/2;
    int ch  = (rows(element)-1)/2;

    float *elem_data = data(float, element);
    // Make structral element as 1d array
    for(h = 0; h < rows(element); h++) {
        for(w = 0; w < cols(element); w++) {
            s = w + cols(element)*h;
            str_elem[s] = (elem_data[w+h*cols(element)] > 0.5);
            str_idxs[s] = (w-cw) + (h- ch)*width(in);
        }
    }

    unsigned char hit;
    #pragma omp parallel for private(h, w, s, hit)
    for(h = ch; h < height(in)-ch; h++) {
        for(w = cw; w < width(in)-cw; w++) {
        // main loop over the image
            hit = 0;
            int idx = w+width(in)*h;
            for(s = 0; s < rows(element)*cols(element); s++) {
                 if ( str_elem[s] && in_data[idx + str_idxs[s]] ) { hit = 255; break; }
            }
            out_data[idx] = hit;
        // compute fit and hit
        }
    }

    return SUCCESS;
}


void resolve(int *list, int u, int v, int N) {

    int i = 0;
    int m = min(list[u], list[v]);
    int n = max(list[u], list[v]);

    for(i=1; i < N; i++) {
        if(list[i]==n) {list[i] = m;}
    }
}

uint32_t fast_ccl(uint8_t *in_data, uint32_t width, uint32_t height, uint32_t *matrix, uint32_t *list)
{

    /* Fast connected component labeling algorithm
    Algorithm is based on two pass scan algorithms
    Inputs:
            matrix: MxN image contains zero and one as Value background(Vb) and Value object (Vo)
            list  : 1xD image which D is the number of maximum object number (max: MxN/4)

    Output: MxN image contains numbers from 0 to D, 0 is the background and D is the maximum object number
    Return: D
    */
    uint8_t Vb = 0; //background

    uint32_t NewLabel = 2; //new label iterator
    uint32_t c1, c2, c3, c4, lx;

    uint32_t i, j, idx;

    // set one pixel border to the output

    // zero out first and last column
    uint32_t colIdx = 0;
    for (j = 0; j < height; j++, colIdx += width)
    {
        matrix[0 + colIdx]  = 0;
        matrix[width - 1 + colIdx]  = 0;
    }

    // zero out first and last row
    uint32_t rowIdx = width * (height - 1);
    for (i = 0; i < width; i++)
    {
        matrix[i] = 0;
        matrix[rowIdx + i] = 0;
    }

    //Provisional labels are assigned to matris
    for (j = 1; j < height - 1; j++)
    {
        idx = j * width;
        for (i = 1; i < width - 1; i++)
        {
            //I(i,j)==1 ?
            if (in_data[i + idx] == Vb)
            {
                matrix[i + idx] = 0;
                continue;
            }

            c1 = matrix[i + idx - 1];
            c2 = matrix[i + idx - 1 - width];
            c3 = matrix[i + idx - width];
            c4 = matrix[i + idx + 1 - width];

            if (c3 != Vb)
            {
                lx = c3;
            }
            else if (c1 != Vb)
            {
                lx = c1;
                if (c4 != Vb)
                {
                    resolve(list, c1, c4, NewLabel);
                }
            } //resolve
            else if (c2 != Vb)
            {
                lx = c2;
                if (c4 != Vb)
                {
                    resolve(list, c2, c4, NewLabel);
                }
            } //resolve
            else if (c4 != Vb)
            {
                lx = c4;
            }
            else
            {
                lx = NewLabel;
                NewLabel++;
            }

            matrix[i + idx] = lx;
        }
    } //end of the i,j loop

    // Create Consecutive Label
    uint32_t max_label = 0;

    for (i = 1; i < NewLabel; i++)
    {
        if (i == list[i])
        {
            list[i] = max_label;
            max_label++;
        }
        else
        {
            list[i] = list[list[i]];
        }
    }

    return max_label - 1;
}

// return number of connected components and index list
/**
 * @brief Compute the connected componenets of the input black and white image
 * @param in Single channel input image with 0 represents the black (background) and 255 represents the white (object)
 * @return Connected components as point_t
*/
vector_t **bwconncomp(matrix_t *in, uint32_t *numElements)
{
    numElements[0] = 0;

    int cond1 = is_image(in) & channels(in) == 1;
    check_condition(cond1, NULL, "input must be uint8 array with single channel, respectively");

    // TODO: create in_data pointer based on the input type
    uint8_t *in_data = data(uint8_t, in);

    // allocate temporary variables
    uint32_t label_length = height(in) * width(in) / 4;

    uint32_t *temp_data = malloc(height(in) * width(in) * sizeof(uint32_t));
    uint32_t *temp_labels = malloc(label_length * sizeof(uint32_t));

    // initialize the list
    uint32_t h;
    for (h = 0; h < label_length; h++)
    {
        temp_labels[h] = h;
    }

    // do the actual computation, single pass connected component labeling
    uint32_t numberOfCC = fast_ccl(in_data, width(in), height(in), temp_data, temp_labels);

    // create output vector
    vector_t **out = malloc(numberOfCC * sizeof(vector_t *));
    for (h = 0; h < numberOfCC; h++)
    {
        out[h] = vector_create(struct point_t);
    }

    // do second pass by resolving the labels
    uint32_t x, y;
    for (y = 0; y < height(in); y++)
    {
        h = y * width(in);
        for (x = 0; x < width(in); x++, h++)
        {
            if (temp_data[h] != 0)
            {
                uint32_t label = temp_labels[temp_data[h]] - 1;

                // create a point and push it to the output vector
                struct point_t pos = point(x, y, 0);
                vector_push(out[label], &pos);
            }
        }
    }

    // free temp variables
    free(temp_data);
    free(temp_labels);

    // prepare output
    numElements[0] = numberOfCC;

    return out;
}

// return number of cc and (L)abels
return_t bwlabel(matrix_t *in, matrix_t *out, uint32_t *numElements)
{
    numElements[0] = 0;

    check_image(in, ERROR_NOT_IMAGE);

    // create matrix container if not existed
    matrix_resize(out, rows(in), cols(in), 1);

    int cond1 =(width(in) == cols(out)) & (height(in) == rows(out)) & (channels(in) == 1);
    check_condition(cond1, ERROR_DIMENSION_MISMATCH, "input and output must have the same dimensions and 2D arrays");

    int cond2 = is_image(in) & (is_32s(out) | is_32u(out));
    check_condition(cond2, ERROR_TYPE_MISMATCH, "input and output must be uint8 and u/int32 arrays, respectively");

    // TODO: create in_data pointer based on the input type
    uint8_t *in_data   = data(uint8_t, in);
    uint32_t *out_data = data(uint32_t, out);

    // allocate temporary variables
    uint32_t label_length = height(in) * width(in) / 4;
    uint32_t *temp_labels = malloc(label_length * sizeof(uint32_t));

    // initialize the list
    uint32_t h;
    for (h = 0; h < label_length; h++)
    {
        temp_labels[h] = h;
    }

    // do the actual computation, single pass connected component labeling
    uint32_t numberOfCC = fast_ccl(in_data, width(in), height(in), out_data, temp_labels);

    // do second pass by resolving the labels
    uint32_t x, y;
    for (y = 0; y < height(in); y++)
    {
        h = y * width(in);
        for (x = 0; x < width(in); x++, h++)
        {
            if (out_data[h] != 0)
            {
                out_data[h] = temp_labels[out_data[h]];
            }
        }
    }

    // free temp variables
    free(temp_labels);

    // prepare output
    numElements[0] = numberOfCC;

    return SUCCESS;
}

return_t label2rgb(matrix_t *in, int inmax, matrix_t *out) {

    // create matrix container if not existed
    matrix_resize(out, rows(in), cols(in), 3);

    int cond1 = (width(out)==cols(in)) && (height(out)==rows(in));
    check_condition(cond1, ERROR_DIMENSION_MISMATCH, "input and output must have the same width and height");

    int cond2 = (channels(in)==1) && ((is_32s(in) || is_32u(in)));
    check_condition(cond2, ERROR_TYPE_MISMATCH, "input must be 2D u/int32 array");

    int cond3 = (channels(out)==3) && is_image(out);
    check_condition(cond3, ERROR_DIMENSION_MISMATCH, "output must be 3 channel uint8 array");

    int i;
    // TODO: create in_data pointer based on the input type
    uint32_t *in_data  = data(uint32_t, in);
    uint8_t  *out_data = data(uint8_t, out);

    // if the maximum of the input is not defined, find it
    if(inmax == 0) {
        for(i=0; i < rows(in)*cols(in); i++) {
            if(in_data[i] > inmax) { inmax = in_data[i]; }
        }
    }
    // create three array to keep random colors
    unsigned char *redch    = (unsigned char*) calloc(inmax, sizeof(unsigned char));
    unsigned char *greench  = (unsigned char*) calloc(inmax, sizeof(unsigned char));
    unsigned char *bluech   = (unsigned char*) calloc(inmax, sizeof(unsigned char));
    // fill the arrays with random color for each input value
    for(i=1; i <= inmax; i++) {
        redch[i]   = rand()%200 + 55;
        greench[i] = rand()%220 + 35;
        bluech[i]  = rand()%180 + 75;
    }
    // make color image from the input values
    for(i=0; i < rows(in)*cols(in); i++) {
        out_data[ 3*i + 0 ]  = redch  [ in_data[i] ];
        out_data[ 3*i + 1 ]  = greench[ in_data[i] ];
        out_data[ 3*i + 2 ]  = bluech [ in_data[i] ];
    }
    free(redch);
    free(greench);
    free(bluech);

    return SUCCESS;
}

/*
// Eucledian distance transform
int fast_edt(uint32_t *grid[2], uint32_t width, uint32_t height) {

    //L(doublet) in the paper
    int c1,c2,c3,c4;
    int i,j, idx;

    uint32_t neighbours[4] = {-1, -width-1, -width, 1-width};
    uint32_t offsetx[4] = {-1, -1, 0, +1};
    uint32_t offsety[4] = {0, -1, -1, -1};

    // first pass of mask1 and mask2
    for(j=1; j < height-1; j++) {
        idx = j*width;
        // one forward pass with mask1
        for(i=1; i < width-1; i++) {
            for(n=0; n < 4; n++) {
                uint32_t dx = grid[0][i + idx + neighbours[n]] + offsetx[n];
                uint32_t dy = grid[1][i + idx + neighbours[n]] + offsety[n];
                uint32_t cd = grid[0][i + idx]*grid[0][i + idx] + grid[1][i + idx]*grid[1][i + idx];

                if((dx*dx+dy*dy)) < cd) {
                    grid[0][i + idx] = dx;
                    grid[1][i + idx] = dy;
                }
            }
        }
        // one backward pass with mask2
        for(i=width-1; i >= 0; i--) {
            uint32_t dx = grid[0][i + idx + 1] + 1;
            uint32_t dy = grid[1][i + idx + 0] + 0;
            uint32_t cd = grid[0][i + idx]*grid[0][i + idx] + grid[1][i + idx]*grid[1][i + idx];

            if((dx*dx+dy*dy)) < cd) {
                grid[0][i + idx] = dx;
                grid[1][i + idx] = dy;
            }
        }
    }

    // second pass of mask3 and mask4
    for(j=height-1; j > 0; j--) {
        idx = j*width;
        // one forward pass with mask3
        for(i=width-1; i > 0; i--) {
            for(n=0; n < 4; n++) {
                uint32_t dx = grid[0][i + idx + neighbours[n]] + offsetx[n];
                uint32_t dy = grid[1][i + idx + neighbours[n]] + offsety[n];
                uint32_t cd = grid[0][i + idx]*grid[0][i + idx] + grid[1][i + idx]*grid[1][i + idx];

                if((dx*dx+dy*dy)) < cd) {
                    grid[0][i + idx] = dx;
                    grid[1][i + idx] = dy;
                }
            }
        }
        // one backward pass with mask4
        for(i=0; i < width-1; i++) {
            uint32_t dx = grid[0][i + idx + 1] + 1;
            uint32_t dy = grid[1][i + idx + 0] + 0;
            uint32_t cd = grid[0][i + idx]*grid[0][i + idx] + grid[1][i + idx]*grid[1][i + idx];

            if((dx*dx+dy*dy)) < cd) {
                grid[0][i + idx] = dx;
                grid[1][i + idx] = dy;
            }
        }
    }

    return 1;
}
*/

/**
"Distance Transform of Sampled Function by Pedro F. Felzenszwalb"
*/
return_t distance_transform(float *distance, uint32_t width, uint32_t height)
{
    uint32_t w,h;
    float inf = 1e24;

    uint32_t *v = (uint32_t*) malloc( (max(width, height)+1) * sizeof(uint32_t)); //(∗ Locations of parabolas in lower envelope ∗)
    float *z = (float*) malloc( (max(width, height)+1) * sizeof(float)); //(∗ Locations of boundaries between parabolas ∗)


    // first pass along the rows
    v[0] = 0;
    z[0] = -inf;
    z[1] = +inf;

    for(h=0; h < height; h++) {

        int k   = 0;
        double s;
        float *function = distance + h*width;
        // process data row by row
        for(w=1; w < width; w++) {
            s = ( (function[w] + w*w) - (function[v[k]] + v[k]*v[k]) )/(2*w - 2*v[k]);
            while (s <= z[k] ) {
                k--;
                s = ( (function[w] + w*w) - (function[v[k]] + v[k]*v[k]) )/(2*w - 2*v[k]);
            }
            k++;

            v[k] = w;
            z[k] = s;
            z[k+1] = +inf;
        }
        k = 0;
        for(w=0; w < width; w++) {
            while (z[k+1] < w) { k++; }
            distance[w + h*width] = (w - v[k])*(w - v[k]) + function[v[k]];
        }
    }

    // second pass along the coloumns
    v[0] = 0;
    z[0] = -inf;
    z[1] = +inf;

    float *function = (float*) malloc(height * sizeof(float));
    for(w=0; w < width; w++) {

        int k   = 0;
        double s;
        for(h=0; h < height; h++) {
            function[h] = distance[w + h*width];
        }
        // process data row by row
        for(h=1; h < height; h++) {
            s = ( (function[h] + h*h) - (function[v[k]] + v[k]*v[k]) )/(2*h - 2*v[k]);
            while (s <= z[k] ) {
                k--;
                s = ( (function[h] + h*h) - (function[v[k]] + v[k]*v[k]) )/(2*h - 2*v[k]);
            }
            k++;

            v[k] = h;
            z[k] = s;
            z[k+1] = +inf;
        }
        k = 0;
        for(h=0; h < height; h++) {
            while (z[k+1] < h) { k++; }
            distance[w + h*width] = (h - v[k])*(h - v[k]) + function[v[k]];
        }
    }

    free(v);
    free(z);
    free(function);

    return SUCCESS;
}

// return number of cc and (L)abels
return_t bwdist(matrix_t *in, matrix_t *out) {

    // create matrix container if not existed
    matrix_resize(out, height(in), width(in), 1);

    int cond1 = (width(in)==cols(out)) && (height(in)==rows(out)) && (channels(in)==1);
    check_condition(cond1, ERROR_DIMENSION_MISMATCH, "input and output must have the same dimensions and 2D arrays");

    int cond2 = is_image(in) && is_32f(out);
    check_condition(cond2, ERROR_TYPE_MISMATCH, "input and output must be uint8 and float arrays, respectively");

    int h;

    // TODO: create in_data pointer based on the input type
    uint8_t *in_data = data(uint8_t, in);
    float *out_data  = data(float, out);

    // fill out the grid
    for(h=0; h < height(in)*width(in); h++) {
        out_data[h] = in_data[h] == 0 ? 0 : max(width(in), height(in));
    }
    // compute the distance transform
    distance_transform(out_data, width(in), height(in));
    return SUCCESS;
}
