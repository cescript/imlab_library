/**
 * @file cvcore.h
 * @author My Self
 * @date 9 Sep 2012
 * @brief File containing example of doxygen usage for quick reference.
 *
 * Here typically goes a more extensive explanation of what the header
 * defines. Doxygens tags are words preceeded by either a backslash @\
 * or by an at symbol @@.
 * @see http://www.stack.nl/~dimitri/doxygen/docblocks.html
 * @see http://www.stack.nl/~dimitri/doxygen/commands.html
 */
#ifndef __IMLAB_CVCORE_H__
#define __IMLAB_CVCORE_H__

#include "core.h"
#include "mlcore.h"

/**
@var int cv_algorithm_t::CV_HOG
Histogram of oriented gradients are commonly used feature descriptors for object detection.
HOG is first proposed by Dallal et.al for pedestrian detection \cite{dalal2005histograms} and Singh et.al for gender classification \cite{singh2013comparison}.
Similar to the LBP features, HOG keeps weighted orientation of the edges in a small sub-block in a compressed histogram vector.

\f$9\f$-bin HOG for each \f$8\times 8\f$ sub-blocks as the feature vector.
In order to compress the magnitude and the angle of the orientation in a \f$9\f$-bin histogram for a given \f$\theta\f$ angle and
\f$M\f$ magnitude, we create the weighted histogram vector $h$ for the \f$bins = [0, 22, 45, 67, 90, ..., 145, 167, 180]\f$ as follows:
\f[ b_1 = \arg\min(bins-\theta > 0)\f] \f[ b_2 = \arg\max(bins-\theta < 0) \f]
\f[ h(b_1) = h(b_1)+ M \frac{b_1-\theta}{b_1-b_2}\f] \f[ h(b_2) = h(b_2)+ M \frac{\theta-b_2}{b_1-b_2}\f]
After the computation of the histogram for each sub-block, we combine the histograms for every \f$2 \times 2\f$ sub-block neighbourhood by adding,
so we reduce the feature vector size by \f$4\f$ times.

<table>
 <tr><th>parameter<th>explanation<th>default value
 <tr><td>\p bins<td>Number of bins in the gradient histogram<td>9
 <tr><td>\p cell[2]<td>Cell size for the histogram of the gradient method. Given image width and height must be divisible bye the cell size<td>[8x8]
 <tr><td>\p block[2]<td>Block size for the Histogram of the gradient method. Each block is a collection of cells<td>[2x2]
 <tr><td>\p stride[2]<td>Overlap between the histogram cells<td>[1x1]
</table>

An example usage
\code{c}
struct feature_t *hog = feature_create(CV_HOG, width(gray),height(gray), "bins:18 block:[6 6] cell:[16 16] stride:[1 1]");
feature_view(hog);
\endcode

*/
/**
@var int cv_algorithm_t::CV_LBP
Local binary patterns are introduced as a texture measure for gray-level images\cite{ojala1996comparative}.
LBP is a feature operator that labels each pixel by thresholding with each of their neighbours so that the situation of the all neighbours can be compressed into the single value.
The mathematical expression of the LBP operator: \f[LBP_{P,R}=\sum_{p=0}^{P-1}u(g_p-g_c)2^p\f] Here \f$u(x)\f$ is a unit function, \f$g_c\f$ is the center pixel that LBP operator is applied,
\f$g_p\f$ is the \f$p^{th}\f$ neighbour in \f$R\f$ neighbourhood and the \f$P\f$ is the number of neighbours.

Generally LBP operator is applied to the given image and then the resulting image is divided into $8\times 8$ sub-blocks by $4$ pixel overlap.
For each $8\times 8$ patch we compute the \f$64\f$-bin histogram. Concatenating these histogram vectors we construct the feature descriptor.

The following parameters are supported for LBP algorithm

<table>
 <tr><th>parameter<th>explanation<th>default value
 <tr><td>\p radius<td>Radius of the LBP<td>1
 <tr><td>\p neighbors<td>Number of neighbors to be visited<td>8
 <tr><td>\p block[2]<td>Block size of the LBP<td>[8x8]
 <tr><td>\p uniform<td>Integer indicating that uniform LBP (1) or normal LBP (0)<td>0
</table>

An example usage
\code{c}
struct feature_t *lbp = feature_create(CV_LBP, width(gray),height(gray), "radius:1 neighbors:8 block:[8 8] uniform:0");
feature_view(lbp);
\endcode
*/
// Extractable features supported by the IMLAB
enum cv_algorithm_t {
    CV_ENCODER = 1,
    CV_LBP  = 2,
    CV_HOG  = 3,
    CV_SIFT = 4,
    CV_NPD  = 5
};

// return type for the feature module
struct feature_t {
    enum cv_algorithm_t algorithm;
    uint32_t feature_size;
    uint32_t image_width;
    uint32_t image_height;
    void *parameters;
    return_t (*method)(matrix_t*, struct feature_t*, float*);
};


/**
 * \brief Construct a feature extraction algorithm with the given parameters
 * @param algorithm Algorithm to be used for feature extraction
 * @param width Width of the input image that the features will be extracted
 * @param height Height of the input image that the features will be extracted
 * @param options Optional algorithm parameters. Parameters that are not given by the \p options are used with the default values.
 * @return feature_t object than can be used for feature extraction
 */
struct feature_t *feature_create(enum cv_algorithm_t algorithm, uint32_t  width, uint32_t height, char *options);

/**
 * \brief Returns the feature size of the model
 * @param model Input model to be used for feature extraction
 * @return Bytes necessary to store the extracted feature
 */
uint32_t  feature_size(struct feature_t *model);
/**
 * \brief Displays the properties of the created model
 * @param model Input model to be displayed
 */
void feature_view(struct feature_t *model);

//
/**
 * \brief Extract the features from the given image and fills the output array
 * @param img Input image that the features will be extracted
 * @param model Input model to be used for feature extraction
 * @param output Output array that the features will be written
 * @return Return success or the corresponding error
 */
return_t feature_extract(matrix_t *img, struct feature_t *model, float *output);
//
/**
 * \brief Visualize the feature if it is possible
 * @param img Input image that the features will be extracted
 * @param model Input model to be used for feature extraction
 * @param output Output image that the visualized features will be written
 * @return Return success or the corresponding error
 */
return_t feature_image(matrix_t *img, struct feature_t *model, matrix_t *output);

/**
 * \brief Read haar classifier from the given json file and return the Haar structure
 * @param filename Filename of the haar classifier
 */
struct haar_t *haar_read(const char *filename);

/**
 * \brief Detects objects using the input cascade.
 * @param img Input image
 * @param cascade Haar cascade read by haar_read
 * @param minSize Minimum size of the search object in pixels
 * @param maxSize Maximum size of the search object in pixels
 * @param scaleFactor Scale factor of the search window
 * @param stepSize Step size between two search window
 */
vector_t *haar_detector(matrix_t *img, struct haar_t *cascade, float minSize, float maxSize, float scaleFactor, float stepSize);

#endif