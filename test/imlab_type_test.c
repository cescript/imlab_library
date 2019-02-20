#include "core.h"
#include "imcore.h"
#include "iocore.h"
#include "prcore.h"

int main() {

    message(SUCCESS, "starting to test IMLAB types");

    // create an output directory
    imlab_mkdir("results");

    uint32_t i,j;

    struct point_t pts1 = point(50,100,5);
    struct point_t pts2 = point(400,200,5);
    // find the distance between the two points
    float d = point_distance(pts1, pts2);

    struct rectangle_t rect1 = rectangle(0,0,2,3, 0);
    struct rectangle_t rect2 = rectangle(0,0,4,6, 0);

    matrix_t *img = matrix_create(uint8_t, 768, 1024, 3);
    struct color_t color = HSV(123,85,40);
    matrix_fill(img, &color);

    for(i=0; i < 100; i++) {
        pts1.x = random_int(10, width(img)-10);
        pts1.y = random_int(10, height(img)-10);

        pts2.x = random_int(10, width(img)-10);
        pts2.y = random_int(10, height(img)-10);

        uint8_t h = random_int(10,60);
        uint8_t s = random_int(120,255);
        uint8_t v = random_int(120,255);
        uint8_t thick = random_int(1,20);
        draw_line(img, pts1, pts2, HSV(h,s,v), thick);
    }

    imwrite(img, "results//line.bmp");
    matrix_free(&img);

    vector_t *file_names = vector_create(string_t);
    char filename[1024];
    for(i=0; i < 1024 * 1000; i++) {
        for(j=0; j < 1023; j++) {
            filename[j] = random_int(10, 60);
        }
        filename[j] = '\0';
        // create a string from the given input
        string_t fname = string(filename);
        vector_push(file_names, &fname);
    }

    vector_free(&file_names);

    message(SUCCESS, "all operations are done successfully!");

    return 1;
}
