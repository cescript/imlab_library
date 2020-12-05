#include <stdlib.h>
#include <string.h>
#include "private/core_private.h"
#include "private/imcore_private.h"
#include "private/lacore_private.h"

return_t imcrop(matrix_t *in, struct rectangle_t crop_region, matrix_t *out) 
{
    int cond1 = is_image(in) & is_image(out);
    check_condition(cond1, ERROR_NOT_IMAGE, "input and output must be an image");

    int cond2 = crop_region.width > 0 && crop_region.height > 0;
    check_condition(cond2, ERROR_DIMENSION_MISMATCH, "width and height of the crop region must be positive");

    // compute the start and end of the crop region
    int32_t x1 = 0, xs = minimum(crop_region.x, width(in) - 1), xe = minimum(xs + crop_region.width - 1, width(in) - 1);
    // if the crop region starts with a negative number, recompute the values
    if (crop_region.x < 0)
    {
        x1 = minimum(-crop_region.x, width(in) - 1);

        xs = 0;
        xe = minimum(crop_region.x + crop_region.width - 1, width(in) - 1);
    }

    // compute the start and end of the crop region
    int32_t y1 = 0, ys = minimum(crop_region.y, height(in) - 1), ye = minimum(ys + crop_region.height - 1, height(in) - 1);
    // if the crop region starts with a negative number, recompute the values
    if (crop_region.y < 0)
    {
        y1 = minimum(-crop_region.y, height(in) - 1);

        ys = 0;
        ye = minimum(crop_region.y + crop_region.height - 1, height(in) - 1);
    }

    // allocate out before use it
    return_t returnValue = matrix_resize(out, crop_region.height, crop_region.width, channels(in));
    check_return(returnValue, returnValue);

    // if we are on the limits, set the out to zero
    if (xs == 0 || ys == 0 || xe == width(in) -1 || ye == height(in) - 1)
    {
        memset(mdata(out,0), 0, volume(out) * elemsize(out));
    }

    uint32_t h;

    uint32_t blockSize = (xe - xs + 1) * channels(in) * elemsize(in);
    uint32_t outputStep = channels(out) * width(out) * elemsize(in);
    uint32_t inputStep = channels(in) * width(in) * elemsize(in);

    // TODO: create in_data pointer based on the input type
    uint8_t *in_data = data(uint8_t, in, ys, xs, 0);
    uint8_t *out_data = data(uint8_t, out, y1, x1, 0);
    
    for (h = 0; h < (ye - ys + 1); h++, out_data += outputStep, in_data += inputStep)
    {
        memcpy(out_data, in_data, blockSize);
    }

    return returnValue;
}

return_t imresize(matrix_t *in, uint32_t nwidth, uint32_t nheight, matrix_t *out) 
{
    int cond1 = is_image(in) & is_image(out);
    check_condition(cond1, ERROR_NOT_IMAGE, "input and output must be an image");

    // allocate out before use it
    matrix_resize(out, nheight, nwidth, channels(in));

    int w,h,d;

    double scalex = width(in)  / (double)nwidth;
    double scaley = height(in) / (double)nheight;

    // TODO: create in_data pointer based on the input type
    uint8_t *in_data  = data(uint8_t, in);
    uint8_t *out_data = data(uint8_t, out);

    int shidx, hidx, swidx, widx;
    #pragma omp parallel for private(shidx, hidx, widx, swidx, h, w, d)
    for(h=0; h < nheight; h++) {
        shidx = h*nwidth;
        hidx  = ((int)(h*scaley))*width(in);
        for(w=0; w < nwidth; w++) {
            widx  = channels(in)*( ((int)(w*scalex)) +hidx);
            swidx = channels(in)*(w+shidx);
            for(d=0; d < channels(in); d++) {
                out_data[swidx + d] = in_data[widx + d];
            }
        }
    }
    //done
    return SUCCESS;
}
/*
Scale2x algorithm written from the wikipedia reference
  A    --\ 1 2
C P B  --/ 3 4
  D
1=P; 2=P; 3=P; 4=P;
IF C==A AND C!=D AND A!=B => 1=A
IF A==B AND A!=C AND B!=D => 2=B
IF D==C AND D!=B AND C!=A => 3=C
IF B==D AND B!=A AND D!=C => 4=D
 */
return_t imscale2x(matrix_t *in, matrix_t *out) 
{
     int cond1 = is_image(in) & is_image(out);
     check_condition(cond1, ERROR_NOT_IMAGE, "input and output must be an image");

     uint32_t nwidth  = 2*width(in);
     uint32_t nheight = 2*height(in);
     // allocate out before use it
     matrix_resize(out, nheight, nwidth, channels(in));

     // TODO: create in_data pointer based on the input type
     uint8_t *in_data  = data(uint8_t, in);
     uint8_t *out_data = data(uint8_t, out);

     int w,h,d;

     int h0, hW, sh01, shW1, sh02, shW2;
     // copy borders directly
     for(h=0; h < height(in); h++) {
         h0 = channels(in)*(h*width(in) + 0);
         hW = channels(in)*(h*width(in) + width(in)-1);

         sh01 = channels(out)*((h+0)*width(out) + 0);
         sh02 = channels(out)*((h+1)*width(out) + 0);

         shW1 = channels(out)*((h+0)*width(out) + width(out)-1);
         shW2 = channels(out)*((h+1)*width(out) + width(out)-1);

         for(d=0; d < channels(in); d++) {
             // first coloumn of the image
             out_data[sh01 + d] = in_data[h0 + d];
             out_data[sh02 + d] = in_data[h0 + d];
             // last coloumn of the image
             out_data[shW1 + d] = in_data[hW + d];
             out_data[shW2 + d] = in_data[hW + d];
         }
     }
     int w0, wH, sw01, swH1, sw02, swH2;
     for(w=0; w < width(in); w++) {
         w0 = channels(in)*(0*width(in) + w);
         wH = channels(in)*((height(in)-1)*width(in) + w);

         sw01 = channels(out)*(0*width(in) + w + 0);
         sw02 = channels(out)*(0*width(in) + w + 1);

         swH1 = channels(out)*((height(out)-1)*width(out) + w + 0);
         swH2 = channels(out)*((height(out)-1)*width(out) + w + 1);

         for(d=0; d < channels(in); d++) {
             // first coloumn of the image
             out_data[sw01 + d] = in_data[w0 + d];
             out_data[sw02 + d] = in_data[w0 + d];
             // last coloumn of the image
             out_data[swH1 + d] = in_data[wH + d];
             out_data[swH2 + d] = in_data[wH + d];
         }
     }

     //uint8_t A,B,C,D,E,F,G,H,I;
     uint8_t B,D,E,F,H;
     uint8_t E0,E1,E2,E3;

     for(h=1; h < height(in)-1; h++) {
         for(w=1; w < width(in)-1; w++) {
             for(d=0; d < channels(in); d++) {
                // get 8 neighbours of w,h,c pixel
                uint32_t in_idx = channels(in)*(h*width(in) + w) + d;
                //A = in_data[in_idx - channels(in)*(width(in)+1)];
                B = in_data[in_idx - channels(in)*(width(in)+0)];
                //C = in_data[in_idx - channels(in)*(width(in)-1)];

                D = in_data[in_idx - channels(in)];
                E = in_data[in_idx];
                F = in_data[in_idx + channels(in)];

                //G = in_data[in_idx + channels(in)*(width(in)-1)];
                H = in_data[in_idx + channels(in)*(width(in)+0)];
                //I = in_data[in_idx + channels(in)*(width(in)+1)];

                if (B != H && D != F) {
                	E0 = D == B ? D : E;
                	E1 = B == F ? F : E;
                	E2 = D == H ? D : E;
                	E3 = H == F ? F : E;
                } else {
                	E0 = E;
                	E1 = E;
                	E2 = E;
                	E3 = E;
                }
                out_data[channels(out)*((2*h+0)*width(out) + 2*w+0) + d] = E0;
                out_data[channels(out)*((2*h+1)*width(out) + 2*w+0) + d] = E2;
                out_data[channels(out)*((2*h+0)*width(out) + 2*w+1) + d] = E1;
                out_data[channels(out)*((2*h+1)*width(out) + 2*w+1) + d] = E3;
             }
         }
     }
     //done
     return SUCCESS;
}




matrix_t *maketform(float data[9]) 
{
    // check that the inpu is inversible
    double det =  data[0]*data[4]*data[8];
           det += data[3]*data[7]*data[2];
           det += data[6]*data[1]*data[5];
           det -= data[0]*data[7]*data[5];
           det -= data[6]*data[4]*data[2];
           det -= data[3]*data[1]*data[8];
    // first create a new empty matrix
    matrix_t *out = matrix_create(float);
    // TODO: define an epsilon here instead of static number
    check_condition(!equal(det, 0, 0.00001), out, "transform matrix is not valid!");

    // do new copy for the output
    matrix_resize(out, 3,3,1);
    det = 1.0f / det;
    // first row
    at(float, out, 0) = det*( data[4]*data[8] - data[5]*data[7]);
    at(float, out, 1) = det*( data[2]*data[7] - data[1]*data[8]);
    at(float, out, 2) = det*( data[1]*data[5] - data[2]*data[4]);
    // second row
    at(float, out, 3) = det*( data[5]*data[6] - data[3]*data[8]);
    at(float, out, 4) = det*( data[0]*data[8] - data[2]*data[6]);
    at(float, out, 5) = det*( data[2]*data[3] - data[0]*data[5]);
    // third row
    at(float, out, 6) = det*( data[3]*data[7] - data[4]*data[6]);
    at(float, out, 7) = det*( data[1]*data[6] - data[0]*data[7]);
    at(float, out, 8) = det*( data[0]*data[4] - data[1]*data[3]);
    // done
    return out;
}

matrix_t *rot2tform(float cx, float cy, float theta, float scale) 
{
    // first create a new empty matrix
    matrix_t *out = matrix_create(float);
    // TODO: define an epsilon here instead of static number
    check_condition(!equal(scale, 0, 0.00001), out, "transform matrix is not valid!");

    float alpha = scale*cosf(deg2rad(theta));
    float beta  = scale*sinf(deg2rad(theta));
    float Tx    = (1-alpha)*cx - beta*cy;
    float Ty    = (beta*cx + (1-alpha)*cy);
    float s     = 1/(scale*scale); // determinant
    // get space for the inverse matrix
    matrix_resize(out, 3,3,1);
    // Since the transformation matrix is known, we return inverse matrix
    float *out_data = data(float, out);
    out_data[0] = alpha*s;
    out_data[1] = -beta*s;
    out_data[2] = (Ty*beta - Tx*alpha)*s;
    out_data[3] = beta*s;
    out_data[4] = alpha*s;
    out_data[5] = -(Tx*beta + Ty*alpha)*s;
    out_data[6] = 0;
    out_data[7] = 0;
    out_data[8] = 1;
    // return the result
    return out;
}

matrix_t *pts2tform(struct point_t *src, struct point_t *dst, int pts_length) 
{
    // do new copy for the output
    matrix_t *inv = matrix_create(float);
    // chack that the inputs are correct
    int cond1 = pts_length == 3 || pts_length ==4;
    check_condition(cond1, inv, "provide 3 or 4 points to find an affine transform!");

    // resize the matrix so that we can insert data in it
    matrix_resize(inv, 3,3,1);

    float *inv_data = data(float, inv);
    inv_data[8] = 1;

    // SOLVE 3 POINT AFFINE (SCALE,ROTATION,SHEAR,TRANSITION) COEFFICIENTS
    if(pts_length == 3) 
    {
        // We should solve 6x6 matrix, since it is a block matrix, we can use simpler method
        float dst_data[] = {dst[0].x, dst[0].y, 1, dst[1].x, dst[1].y, 1, dst[2].x, dst[2].y, 1};

        // get inverse of the created matrix
        matrix_t *invA = maketform(dst_data);
        float *invA_data = data(float, invA);

        inv_data[0] = src[0].x*invA_data[0]+src[1].x*invA_data[1]+src[2].x*invA_data[2];
        inv_data[1] = src[0].x*invA_data[3]+src[1].x*invA_data[4]+src[2].x*invA_data[5];
        inv_data[2] = src[0].x*invA_data[6]+src[1].x*invA_data[7]+src[2].x*invA_data[8];
        inv_data[3] = src[0].y*invA_data[0]+src[1].y*invA_data[1]+src[2].y*invA_data[2];
        inv_data[4] = src[0].y*invA_data[3]+src[1].y*invA_data[4]+src[2].y*invA_data[5];
        inv_data[5] = src[0].y*invA_data[6]+src[1].y*invA_data[7]+src[2].y*invA_data[8];
        inv_data[6] = 0;
        inv_data[7] = 0;
    
        //do something here!
        matrix_free(&invA);
    }
    else if(pts_length == 4) 
    {
        float dst_data[] = 
        {
            dst[0].x, dst[0].y, 1, 0, 0, 0, -src[0].x * dst[0].x, -src[0].x * dst[0].y,
            0, 0, 0, dst[0].x, dst[0].y, 1, -src[0].y * dst[0].x, -src[0].y * dst[0].y,

            dst[1].x, dst[1].y, 1, 0, 0, 0, -src[1].x * dst[1].x, -src[1].x * dst[1].y,
            0, 0, 0, dst[1].x, dst[1].y, 1, -src[1].y * dst[1].x, -src[1].y * dst[1].y,

            dst[2].x, dst[2].y, 1, 0, 0, 0, -src[2].x * dst[2].x, -src[2].x * dst[2].y,
            0, 0, 0, dst[2].x, dst[2].y, 1, -src[2].y * dst[2].x, -src[2].y * dst[2].y,

            dst[3].x, dst[3].y, 1, 0, 0, 0, -src[3].x * dst[3].x, -src[3].x * dst[3].y,
            0, 0, 0, dst[3].x, dst[3].y, 1, -src[3].y * dst[3].x, -src[3].y * dst[3].y,

        };

        float b_data[] = {src[0].x, src[0].y, src[1].x, src[1].y, src[2].x, src[2].y, src[3].x, src[3].y};

        matrix_t *inA = matrix_create(float, 8, 8, 1, dst_data);
        matrix_t *inB = matrix_create(float, 8, 1, 1, b_data);

        matrix_divide(inA, inB, inv);

        matrix_free(&inA);
        matrix_free(&inB);
    }

    return inv;
}

struct point_t apply_tform(struct point_t in, matrix_t *tform) 
{
    matrix_t *itform = maketform(data(float, tform));
    float *tr_data = data(float, tform);

    struct point_t out = {0, 0};

    double zp = in.x*tr_data[6] + in.y*tr_data[7] + tr_data[8];
    // TODO: define an epsilon here instead of static number
    if(equal(zp, 0, 0.00001)) { return out; }

    out.x = (in.x*tr_data[0] + in.y*tr_data[1] + tr_data[2]) / zp;
    out.y = (in.x*tr_data[3] + in.y*tr_data[4] + tr_data[5]) / zp;

    matrix_free(&itform);
    return out;
}

return_t imtransform(matrix_t *in, matrix_t *tr, matrix_t *out) 
{

    int cond1 = is_image(in) & is_image(out);
    check_condition(cond1, ERROR_NOT_IMAGE, "input and output must be an image");

    // get the size of the output, if it has a size than use it
    uint32_t nwidth  = width(out);
    uint32_t nheight = height(out);

    // otherwise decide the size based on the input size
    if(nwidth==0 && nheight==0) 
    {
        // TODO: auto find the meaningful image region and create it
        /*
        // auto find new width and height UL,UR,BL,BR corners of the input
        point_t pin[4] = {0,0, width(in),0, width(in),height(in), 0,height(in)};
        point_t pout = apply_tform(pin[0], tr);
        int xmin = pout.x;
        int xmax = pout.x;
        int ymin = pout.y;
        int ymax = pout.y;

        int c = 0;
        for(c=1; c < 4; c++) {
            pout = apply_tform(pin[c], tr);
            //printf("%3.2f %3.2f\n", pout[c].x, pout[c].y);
            if(xmin > pout.x) { xmin = pout.x; }
            if(xmax < pout.x) { xmax = pout.x; }
            if(ymin > pout.y) { ymin = pout.y; }
            if(ymax < pout.y) { ymax = pout.y; }
        }
        // consider xmin|ymin < 0
        nwidth  = xmax-xmin;
        nheight = ymax-ymin;
        */
        nwidth  = width(in);
        nheight = height(in);
    }
    matrix_resize(out, nheight, nwidth, channels(in));

    // TODO: create in_data pointer based on the input type
    uint8_t *in_data  = data(uint8_t, in);
    uint8_t *out_data = data(uint8_t, out);

    int w,h;
    // Affine
    int shidx, x, y, d;
    float *tr_data = data(float, tr);

    //#pragma omp parallel for private(shidx, x, y, h, w, d)
    for(h=0; h < height(out); h++) 
    {
        shidx = h*width(out);

        for(w=0; w < width(out); w++) 
        {
            double zp = w*tr_data[6] + h*tr_data[7] + tr_data[8];
            // TODO: define an epsilon here instead of static number
            if(equal(zp, 0, 0.00001)) { continue; }

            x = (w*tr_data[0] + h*tr_data[1] + tr_data[2]) / zp;
            y = (w*tr_data[3] + h*tr_data[4] + tr_data[5]) / zp;

            //printf("coor: %d --> %d\t %d --> %d\n", x,w, y,h);
            // check whether the x,y is outside the image
            if( (x < 0) || (y < 0) || (x > width(in)-1) || (y > height(in)-1) ) { continue; }

            for(d=0; d < channels(in); d++) 
            {
                out_data[channels(out)*(w+shidx) + d] = in_data[ channels(in)*(x+width(in)*y) + d];
            }
        }
    }
    return SUCCESS;
}
