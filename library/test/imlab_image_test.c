// test all functionalities of the library
#include <stdio.h>
#include <stdint.h>
#include "imcore.h"
#include "prcore.h"
#include "iocore.h"
#include <time.h>

int main()
{
    message(SUCCESS, "starting to test IMLAB image library");

    // create an output directory
    imlab_mkdir("results");

    int i,j,c;

    double fill_inf[3] ={1,2,3.3};

    matrix_t *img   = imread("../test/test_data/sample.bmp");
    imwrite(img, "results//input_copy.bmp");
    // create an image display with the given name and property
    struct window_t *display = window_create("IMLAB IMAGE  DISPLAY TEST", 0);
    // display the image in the created window
    matrix_t *displayImg = matrix_create(uint8_t);


    clock_t start = clock();
    for(i = 0; i < 0; i++) {
        // create an image
        int W = random_int(300, 1000);
        int H = W / random_float(1,3);

        int D = random_int(0,1) == 0 ? 1:3;

        matrix_resize(displayImg, H, W, D);

        int hue = random_int(0  , 255);
        int sat = random_int(100, 255);
        int val = random_int(100, 255);

        struct color_t color = HSV(hue,sat,val);
        matrix_fill(displayImg, &color);

        imshow(displayImg, display);
        // wait for a while to complete the display procedure
        window_wait(display, 10);
    }
    int end = clock();
    printf("Image display took %3.2f ms\n", 1000*(double)(end-start) / CLOCKS_PER_SEC );


    // create an empty matrix
    matrix_t *img2 = matrix_create(img, NULL);
    imload("../test/test_data/sample.bmp", img2);
    // show the image
    imshow(img2, display);
    window_wait(display, 1000);

    matrix_view(img2);
    matrix_free(&img2);

    matrix_t *gray  = matrix_create(uint8_t, height(img), width(img), 1);
    matrix_t *bwimg = matrix_create(gray, data(void, gray)); // create a matrix from the gray matrix
    matrix_t *img2x = matrix_create(uint8_t);
    matrix_t *label = matrix_create(uint32_t);

    rgb2gray(img, gray);
    imwrite(gray, "results//out_gray.bmp");
    imscale2x(img, img2x);
    imwrite(img2x,"results//scaled2x.bmp");

    // test color transforms
    imthreshold(gray, imotsu(gray), bwimg);
    imwrite(bwimg, "results//thresholded.bmp");

    uint32_t conn = 0;
    bwlabel(bwimg, label, &conn);
    printf("Label: %d\n", conn);
    label2rgb(label, conn, img);

    // show the image
    imshow(img, display);
    window_wait(display, 1000);
    imwrite(img, "results//bwlabel.bmp");

    matrix_t *dist = matrix_create(float);
    bwdist(bwimg, dist);
    matrix2image(dist, 0, gray);
    imwrite(gray, "results//distance_transform.bmp");

    // test colorization

    struct color_t color = HSV(238,170,255);
    matrix_view(img2x);
    matrix_fill(img2x, &color);
    imwrite(img2x, "results//hsv_colorization.bmp");
    // test geometric transform functions
    //point_t src[3] = {0,0,   img->width,0,  img->width,img->height};
    //point_t dst[3] = {0,0, img->width,0, img->width ,img->height/2};
    //matrix_t tform = pts2tform(src, dst, 3); // create TFORM from the given array
    //float data[] = {0.5,0,0, 0,0.5,0, 0,0,1};
    //matrix_t tform = maketform( data ); // create TFORM from given affine matrix
    matrix_t *tform = rot2tform(width(img)/2, height(img)/2, 45, 0.5);
    matrix_free(&img2x);
    img2x = matrix_create(uint8_t);
    rgb2any(img, RGB2SEPIA, img);

    imtransform(img, tform, img2x);

    // show the image
    imshow(img2x, display);
    window_wait(display, 1000);
    imwrite(img2x, "results//transformed.bmp");

    // test integral image
    matrix_free(&bwimg);
    bwimg = matrix_create(uint8_t, 400, 600, 1);
    color = RGB(128,128,128);
    matrix_fill(bwimg, &color);

    // create two different floating point matrce
    matrix_t *mat0x0f1 = matrix_create(float);
    matrix_t *mat0x0f2 = matrix_create(float);

    integral(bwimg, mat0x0f1, mat0x0f2);
    matrix2image(mat0x0f1, 0, bwimg);
    // test imwrite
    imwrite(bwimg, "results//integral_image_result.bmp");


    message(SUCCESS, "all operations are done successfully!");

    return 0;
}
