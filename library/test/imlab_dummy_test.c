// test all functionalities of the library
#include <stdio.h>
#include <stdint.h>
#include "core.h"
#include <time.h>
#include "iocore.h"
#include "imcore.h"

int main()
{
    imlab_mkdir("results");

    char filename[128];

    uint8_t value[3] = {255, 255, 0};
    matrix_t *img   = matrix_create(uint8_t, 100, 100, 3);
    matrix_fill(img, value);

    struct rectangle_t b[10];

    b[0] = rectangle(-10, -10, +20, +20, 1);
    b[1] = rectangle(+40, -10, +20, +20, 1);
    b[2] = rectangle(+90, -10, +20, +20, 1);
    b[3] = rectangle(+90, +40, +20, +20, 1);
    b[4] = rectangle(+90, +90, +20, +20, 1);
    b[5] = rectangle(+40, +90, +20, +20, 1);
    b[6] = rectangle(-10, +90, +20, +20, 1);
    b[7] = rectangle(-10, +40, +20, +20, 1);
    b[8] = rectangle(-10, -10, 120, 120, 1);

    // save cropped images
    for(int i = 0; i < 9; i++) {

        matrix_t *crop = matrix_create(uint8_t);

        imcrop(img, b[i], crop);

        sprintf(filename, "results//images_%02d.bmp", i);
        imwrite(crop, filename);

        matrix_free(&crop);
    }

    message(SUCCESS, "all operations are done successfully!");

    return 0;
}
