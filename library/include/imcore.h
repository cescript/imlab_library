#ifndef __IMLAB_IMCORE_H__
#define __IMLAB_IMCORE_H__

// Include IMLAB general headers
#include "core.h"
#include <math.h>


// GLOBAL DEFINITIONS //
static const float RGB2SEPIA[3][4] = { {0.393, 0.769, 0.189, 0}, {0.349, 0.686, 0.168, 0}, {0.272, 0.534, 0.131, 0} };
static const float RGB2YIQ  [3][4] = { {0.299, 0.587, 0.114, 0}, {0.596, -0.275, -0.321, 128}, {0.212, -0.523, 0.311, 128} };
static const float YIQ2RGB  [3][4] = { {1.0, 0.956, 0.621, -201.856}, {1.0, -0.272, -0.647, 117.632}, {1.0, -1.107, 1.704, -76.416} };
static const float RGB2YCC  [3][4] = { {0.299, 0.587, 0.114, 0}, {0.5, -0.419, -0.081, 128}, {-0.169, -0.331, 0.5, 128} };
static const float YCC2RGB  [3][4] = { {1.0, 1.402, 0.0, -179.456}, {1.0, -0.714, -0.344, 135.424}, {1.0, 0, 1.772, -226.816} };

struct color_t {
    uint8_t blue, green, red;
};

struct color_t RGB(uint8_t r, uint8_t g, uint8_t b);
struct color_t HSV(uint8_t h, uint8_t s, uint8_t v);

// convert the given RGB color into grayscale
#define RGB2GRAY(c) ( (uint8_t) ( (76 * (uint32_t)(c).red + 151 * (uint32_t)(c).green + 29 * (uint32_t)(c).blue) >> 8 ) )

/*
#define color_rgb_split(c, r,g,b) ((r)= (c>>16)&&0xFF, (g)= (c>>8)&&0xFF, (b) = (c)&&0xFF;)
#define _get_colorGRAY(in, idx) _color_RGB2INT((in)->data[(idx)],(in)->data[(idx)],(in)->data[(idx)])
#define _get_colorRGB(in, idx) ((in)->data[3*((w) + (in)->width*(h))+2] << 16 | (in)->data[3*((w) + (in)->width*(h))+1] << 8 | (in)->data[3*((w) + (in)->width*(h))+0])
*/

/* Define Image Processing functions */

//image_io.c
/**
 * Read the given image file and return the output matrix.
 * @param filename Input image to be read
 * @return
 */
matrix_t *imread(char *filename);
/**
 * Load the given image file into the given output matrix. The output matrix should have the same size with the given image.
 * This method is useful if you read images with the same & known sizes in a loop.
 * @param filename Input image to be read
 * @param out Output matrix to be filled
 * @return
 */
return_t imload(char *filename, matrix_t *out);
/**
 * Write the image into the given filename
 * @param in Input image to be written
 * @param filename Output filename for the image
 * @return
 */
return_t imwrite(matrix_t *in, char *filename);
return_t matrix2image(matrix_t *in, uint8_t opt, matrix_t *out);

// image display functions
/**
 * Create a window to display the image
 * @param windowname Window name to be displayed on the window name bar
 * @param options Window options (Not supported).
 * @return Return a window object that can imshow can write images onto it
 */
struct window_t* window_create(char *windowname, uint32_t options);

/**
 * Display the given image onto the given window
 * @param image IMLAB image to be displayed
 * @param display Window object which is allocated by window_create
 */
void imshow(matrix_t *image, struct window_t *display);

/**
 * Wait for #sleep milliseconds to display the image.
 * @param display Window object which is allocated by window_create
 * @param sleep Sleep duration to show the image
 * @return Return the pressed key or mouse button during the wait
 */
int window_wait(struct window_t *display, int sleep);

//image_bw.c
return_t bwlabel(matrix_t *in, matrix_t *label, uint32_t *numElements);

// return number of connected components and index list
/**
 * @brief Compute the connected componenets of the input black and white image
 * @param in Single channel input image with 0 represents the black (background) and 255 represents the white (object)
 * @return Connected components as point_t
*/
vector_t **bwconncomp(matrix_t *in, uint32_t *numElements);

return_t imerode(matrix_t *in, matrix_t *str, matrix_t *out);
return_t imdilate(matrix_t *in, matrix_t *str, matrix_t *out);
return_t label2rgb(matrix_t *in, int inmax, matrix_t *out);
return_t bwdist(matrix_t *in, matrix_t *out);
//
//    CELL *bwconncomp(matrix_tI);
//    some more morphological operations
//
//color_transform.c
return_t histogram(matrix_t *in, int32_t min, int32_t max, uint32_t **histogram);
uint32_t imotsu(matrix_t *in);
return_t imbinarize(matrix_t *in, uint32_t block_width, uint32_t block_height, float threshold, matrix_t *out);
return_t imthreshold(matrix_t *in, uint32_t threshold, matrix_t *out);
return_t rgb2gray(matrix_t *in, matrix_t *out);
return_t gray2rgb(matrix_t *in, matrix_t *out);
return_t rgb2hsv(matrix_t *in, matrix_t *out);
return_t hsv2rgb(matrix_t *in, matrix_t *out);
return_t rgb2any(matrix_t *in, const float tr[3][4], matrix_t *out);
void rgb2lab(struct color_t C1, float *L, float *a, float *b);

return_t image_set(matrix_t *in, uint32_t y, uint32_t x, struct color_t clr);
return_t image_get(matrix_t *in, uint32_t y, uint32_t x, struct color_t *clr);
//geometric_transform.c
return_t imcrop(matrix_t *in, struct rectangle_t crop_region, matrix_t *out);
return_t imresize(matrix_t *in, uint32_t nwidth, uint32_t nheight, matrix_t *out);
return_t imscale2x(matrix_t *in, matrix_t *out);
matrix_t *maketform(float data[9]);
matrix_t *rot2tform(float cx, float cy, float theta, float scale);
matrix_t *pts2tform(struct point_t *src, struct point_t *dst, int len);
return_t imtransform(matrix_t *in, matrix_t *tform, matrix_t *out);

return_t integral(matrix_t *in, matrix_t *sums, matrix_t *ssum);
return_t box_filter(matrix_t *in, uint32_t block_width, uint32_t block_height, matrix_t *out);
// find the median filtering result of the input image
return_t medfilt2(matrix_t *input, uint32_t filter_width, uint32_t filter_height, matrix_t *output);

// integral image functions
float integral_get_float(matrix_t *in, int x0, int y0, int x1, int y1, int c);
double integral_get_double(matrix_t *in, int x0, int y0, int x1, int y1, int c);


// drawing functions
void draw_point(matrix_t *img, struct point_t p1, struct color_t clr, int thickness);
// draw line using Bressnam algorithm
void draw_line(matrix_t *img, struct point_t p1, struct point_t p2, struct color_t clr, int thickness);
// draw rectangle using four lines
void draw_rectangle(matrix_t *img, struct rectangle_t r, struct color_t clr, int thickness);


#endif