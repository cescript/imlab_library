#include <stdlib.h>
#include "private/core_private.h"
#include "private/imcore_private.h"


void draw_line(matrix_t *img, struct point_t p1, struct point_t p2, struct color_t clr, int thickness) {

    // get the difference between the points
    int x1 = p1.x;
    int y1 = p1.y;
    int x2 = p2.x;
    int y2 = p2.y;

    int incX = x1 < x2 ? 1 : -1;
    int incY = y1 < y2 ? 1 : -1;
    int deltaX = abs(x2 - x1);
    int deltaY = abs(y2 - y1);
    int delta = deltaX - deltaY;
    int error = 0;
    float ed = (deltaX+deltaY) == 0 ? 1:sqrt((float)deltaX*deltaX+(float)deltaY*deltaY);

    int xk, yk;
    for (;;)
    {
        error = delta;
        xk = x1;
        // put point (xk,yk) to the image
        image_set(img, y1, x1, clr);

        if (2*error >= -deltaX) {
            for (error += deltaY, yk = y1; error < ed*thickness && (y2 != yk || deltaX > deltaY); error += deltaX) {
                image_set(img, yk += incY, x1, clr);
            }
            if (x1 == x2) break;
            error = delta;
            delta -= deltaY;
            x1 += incX;
        }
        if (2*error <= deltaY) {
            for (error = deltaX-error; error < ed*thickness && (x2 != xk || deltaX < deltaY); error += deltaY) {
                image_set(img, y1, xk += incX, clr);
            }
            if (y1 == y2) break;
            delta += deltaX;
            y1 += incY;
        }
    }
    return;
};

// draw rectangle to the given image
void draw_rectangle(matrix_t *img, struct rectangle_t r, struct color_t clr, int thickness) 
{
    int hthickness = (int)((thickness - 1) / 2);
    thickness = 2 * hthickness + 1;

    // draw four line
    int x0 = r.x - hthickness;
    int y0 = r.y - hthickness;

    int x1 = r.x + hthickness;
    int y1 = r.y + hthickness;

    int x2 = r.x + r.width + hthickness;
    int y2 = r.y + r.height + hthickness;

    // draw four line
    draw_line(img, point(x0, y1, 0), point(x2, y1, 0), clr, thickness);
    draw_line(img, point(x2, y0, 0), point(x2, y2, 0), clr, thickness);
    draw_line(img, point(x2, y2, 0), point(x0, y2, 0), clr, thickness);
    draw_line(img, point(x1, y2, 0), point(x1, y0, 0), clr, thickness);
};

void draw8(matrix_t *img, int x, int y, int xcenter, int ycenter, struct color_t clr)
{
    image_set(img, +y + ycenter, +x + xcenter, clr);
    image_set(img, +y + ycenter, -x + xcenter, clr);
    image_set(img, -y + ycenter, +x + xcenter, clr);
    image_set(img, -y + ycenter, -x + xcenter, clr);

    image_set(img, +x + ycenter, +y + xcenter, clr);
    image_set(img, +x + ycenter, -y + xcenter, clr);
    image_set(img, -x + ycenter, +y + xcenter, clr);
    image_set(img, -x + ycenter, -y + xcenter, clr);
}

// circle rasterization with only integer arithmetic
// copied from: http://www.mindcontrol.org/~hplus/graphics/RasterizeCircle.html
void draw_point(matrix_t *img, struct point_t p1, struct color_t clr, int thickness)
{
    int radius = (thickness + 1) / 2;

    // necessary variables for circle rasterization, only needed for comparsions
    int radiusSquare = radius * radius * 4; 
    int xPlus1Square = 0;
    int yPlus1Square = 0;
    int yMinus1Square = radiusSquare - 2 *radius + 1;

    int x = 0;
    int y = radius;

    draw8(img, x, y, p1.x, p1.y, clr);

    while (x <= y)
    {
        // find the 4 times square of (x+1) using the previous value
        xPlus1Square = xPlus1Square + 8 * x + 4;

        // advance to the right
        ++x;

        // find the 4 times square of (y+1) which is 4*(y+1)*(y+1) : 4*(r*r) - 4*(x+1)*(x+1) => yPlus1Square = radiusSquare - xPlus1Square
        yPlus1Square = radiusSquare - xPlus1Square;

        // decide keep Y or decrement Y
        if (yPlus1Square < yMinus1Square)
        {
            // find the 4 times square of (y-1) using the previous value
            yMinus1Square = yMinus1Square - 8 * y + 4;

            // decrement the y value
            --y;
        }

        // draw 8 symmetric circle points
        draw8(img, x, y, p1.x, p1.y, clr);
    }
    

    return;
};
