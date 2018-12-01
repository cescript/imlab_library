// test all functionalities of the library
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "cvcore.h"
#include "imcore.h"
#include "iocore.h"

int main()
{

    message(SUCCESS, "starting to test IMLAB computer vision library");

    int i,j,c;
    // create an output directory
    imlab_mkdir("results");

    matrix_t *img   = imread("../test/test_data/sample.bmp");

    matrix_t *gray  = matrix_create(uint8_t, height(img), width(img), 1);
    rgb2gray(img, gray);

    struct feature_t *model = feature_create(CV_LBP, width(gray),height(gray), "");


    matrix_t *lbp_img = matrix_create(int32_t);
    //lbp_image(gray, lbp_img, model->parameters);
    matrix_convert(int32_t, uint8_t, lbp_img);
    imwrite(lbp_img, "results//lbp_image.bmp");


    double fill_inf[3] ={1,2,3.3};


    float data[4] = {1.1, 2.2, 3.3, 4.4};
    matrix_t *matA = matrix_create(float, 1,4,1, data);
    // print the type name
    printf("Type of matA: %s\n", typename(matA));
    // print the values
    printf("[%3.2f %3.2f %3.2f %3.2f]\n", at(float, matA, 0), at(float, matA, 1), at(float, matA, 2), at(float, matA, 3));
    // convert the values into int type
    matrix_convert(float, int32_t, matA);
    // print the type name
    printf("Type of matA: %s\n", typename(matA));
    // print the values
    printf("[%d %d %d %d]\n", at(int32_t, matA, 0), at(int32_t, matA, 1), at(int32_t, matA, 2), at(int32_t, matA, 3));

    string_t stream = string("");

    string_printf(&stream, "hello");
    string_printf(&stream, "%c", 32);
    string_printf(&stream, "IML@B");

    printf("%s\n", c_str(stream));

    // create an LBP model with the default parameters
    struct feature_t *lbp = feature_create(CV_LBP, width(gray),height(gray), "");
    feature_view(lbp);

    // create a HOG model with the given parameters
    struct feature_t *hog = feature_create(CV_HOG, width(gray),height(gray), "bins:18 stride:1x1 block:6x6 cell:16x16");
    feature_view(hog);


    float *feature = (float*)malloc(feature_size(hog) * sizeof(float));
    feature_extract(gray, hog, feature);
    //matrix_t *hog_img = hog2image(feature, hog->parameters);
    //printf("hog_img size: %d x %d\n", width(hog_img), height(hog_img));
    //imwrite(hog_img, "results//hog_image.bmp");

    message(SUCCESS, "all operations are done successfully!");

    return 0;
}
