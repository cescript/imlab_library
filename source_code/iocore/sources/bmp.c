//
// Created by cescript on 13.04.2018.
//
#include <stdio.h> // for file io
#include <stdint.h> // for int types
#include <stdlib.h> // for calloc and malloc
#include "private/iocore_private.h"

// bitmap info header
struct bitmap_info_header_t {
    uint32_t bisize; //bitmap info size
    uint32_t width;  //x pixels of bmp
    uint32_t height; //y pixels of bmp
    uint16_t  biplane;//number of planes set to 1
    uint16_t  bppixel;//bit per pixel
    uint32_t comtype;//compression method
    uint32_t rawsize;//total bytes of color and padding
    uint32_t xpmeter;//x pixel per meter-horizontal resolution
    uint32_t ypmeter;//y pixel per meter-vertical   resolution
    uint32_t ncolors;//number of colors in the palette
    uint32_t icolors;//important colors
};

// bitmap header
struct bitmap_header_t {
    uint8_t  bftype[2];  //bitmap indicator must be 'BM'
    uint32_t bfsize;  //bitmap file size
    uint16_t  bfres1;  //reseved bytes
    uint16_t  bfres2;  //reserved bytes
    uint32_t bfoffset;//start byte of colors
    struct bitmap_info_header_t info; // information header
};

// read the given bmp file into the allocated memory
void read_bmp_data(FILE *bmp, struct bitmap_header_t header, uint8_t *data) {

    uint32_t colormap_length = 0;
    uint8_t *colormap = NULL;
    uint32_t input_channels = 3;
    // allocate the colormap and set the corresponding variable if it is an indexed image
    if(header.info.bppixel == 8) {
        input_channels = 1;
        colormap_length = header.info.ncolors == 0 ? 256 : header.info.ncolors;
        colormap = calloc(colormap_length * 4, sizeof(uint8_t));
    }

    // get the copy of the height
    uint32_t input_height = header.info.height;
    // input row length as bytes
    uint32_t input_row_length = input_channels * header.info.width;

    // read the colormap if it is an indexed image
    if (header.info.bppixel == 8)
    {
        // read the color table (rgba)
        fread(colormap, sizeof(uint8_t), colormap_length * 4, bmp);
    }

    // get the padding size
    uint32_t padding = input_row_length % 4;
    if(padding) {
        padding = (4 - padding);
    }

    // read the rows into the buffer in reverse order
    uint32_t padding_bytes;
    data += (input_height-1) * 3 * header.info.width;

    while(input_height--) {
        // read a single line from the source
        fread(data , sizeof(uint8_t), input_row_length, bmp);
        // decode if it is indexed image
        if(colormap != NULL) {
            uint32_t w = header.info.width;
            do
            {
                uint32_t colorIDX = data[--w];

                data[3*w + 0] = colormap[4*colorIDX + 0];
                data[3*w + 1] = colormap[4*colorIDX + 1];
                data[3*w + 2] = colormap[4*colorIDX + 2];
            }
            while(w != 0);
        }
        // continue with the next line
        fread(&padding_bytes, sizeof(uint8_t), padding, bmp);
        data -= 3 * header.info.width;
    }
    if(colormap != NULL) {
        free(colormap);
    }
    return;
}

// write the given bmp file into the allocated memory
void write_bmp_data(FILE *bmp, struct bitmap_header_t header, uint8_t *data, uint8_t *colormap) {

    uint32_t input_channels = 3;
    // allocate the colormap and set the corresponding variable if it is an indexed image
    if(header.info.bppixel == 8) {
        input_channels = 1;
    }

    // get the copy of the height
    uint32_t input_height = header.info.height;
    // input row length as bytes
    uint32_t input_row_length = input_channels * header.info.width;

    // write the colormap if it is an indexed image
    if(header.info.bppixel == 8) {
        // read the color table (rgba)
        fwrite(colormap, sizeof(uint8_t), header.info.ncolors * 4, bmp);
    }

    // compute the padding size
    uint32_t padding = input_row_length % 4;
    if(padding) {
        padding = (4 - padding);
    }

    // read the rows into the buffer in reverse order
    uint32_t padding_bytes = 0;
    data += (input_height-1) * input_row_length;
    while(input_height--) {
        // read a single line from the source
        fwrite(data , sizeof(uint8_t), input_row_length, bmp);
        // continue with the next line
        fwrite(&padding_bytes, sizeof(uint8_t), padding, bmp);
        data -= input_row_length;
    }

    return;
}


// read the bitmap from the given filename fill the header and data pointers
return_t read_bmp(char *filename, uint8_t **data, uint32_t *img_width, uint32_t *img_height, uint32_t *img_channels) {
    // open the file
    FILE *bmp = fopen(filename,"rb");
    check_file(bmp, ERROR_UNABLE_TO_OPEN);
    // read the bmp header
    long int size = get_file_size(bmp, 0);
    // check the file size and make sure it is not corrupted
    if(size  < sizeof(struct bitmap_header_t)) {
        fclose(bmp);
        return ERROR_TYPE_MISMATCH;
    }
    // create a bitmap header
    struct bitmap_header_t header;
    // if the magic bytes are correct assume that the given file is an image
    fread(header.bftype, sizeof(uint8_t), 2, bmp);
    if(header.bftype[0] != 'B' || header.bftype[1] != 'M') {
        fclose(bmp);
        return ERROR_TYPE_MISMATCH;
    }
    // check the given file's size in the header and compare it with the real size
    fread(&header.bfsize, sizeof(uint32_t), 1, bmp);
    if(header.bfsize > size) {
        fclose(bmp);
        return ERROR_TYPE_MISMATCH;
    }
    fread(&header.bfres1, sizeof(uint16_t), 1, bmp);
    fread(&header.bfres2, sizeof(uint16_t), 1, bmp);
    fread(&header.bfoffset, sizeof(uint32_t), 1, bmp);
    // read the header
    fread(&header.info.bisize, sizeof(uint32_t), 1, bmp);
    fread(&header.info.width , sizeof(uint32_t), 1, bmp);
    fread(&header.info.height, sizeof(uint32_t), 1, bmp);
    fread(&header.info.biplane, sizeof(uint16_t), 1, bmp);
    fread(&header.info.bppixel, sizeof(uint16_t), 1, bmp);
    fread(&header.info.comtype, sizeof(uint32_t), 1, bmp);
    fread(&header.info.rawsize, sizeof(uint32_t), 1, bmp);
    fread(&header.info.xpmeter, sizeof(uint32_t), 1, bmp);
    fread(&header.info.ypmeter, sizeof(uint32_t), 1, bmp);
    fread(&header.info.ncolors, sizeof(uint32_t), 1, bmp);
    fread(&header.info.icolors, sizeof(uint32_t), 1, bmp);

    // allocate bytes for the output data
    if(data[0] == NULL) {
        data[0] = (uint8_t*) calloc(3 * header.info.width * header.info.height, sizeof(uint8_t));
    } else if( img_channels[0] != 3 || img_width[0] != header.info.width || img_height[0] != header.info.height) {
        fclose(bmp);
        return ERROR_DIMENSION_MISMATCH;
    }
    // check the given/allocated memory and return error if it is NULL
    check_memory(data[0]) {
        fclose(bmp);
        return ERROR_OUT_OF_MEMORY;
    }
    // set the output variables
    img_width[0]    = header.info.width;
    img_height[0]   = header.info.height;
    img_channels[0] = 3;
    // get ready for the reading
    if(header.info.comtype == 0) {
        // read uncompressed bitmap data
        if(header.info.bppixel == 8 || header.info.bppixel == 24) {
            read_bmp_data(bmp, header, data[0]);
        }
        else {
            fclose(bmp);
            return WARNING_NOT_SUPPORTED;
        }
    } else {
        fclose(bmp);
        return WARNING_NOT_SUPPORTED;
    }
    fclose(bmp);
    // tell that we read the image
    return SUCCESS;
}

// read the bitmap from the given filename fill the header and data pointers
return_t write_bmp(char *filename, uint8_t *data, uint32_t img_width, uint32_t img_height, uint32_t img_channels) {
    // open the file
    FILE *bmp = fopen(filename,"wb");
    check_file(bmp, ERROR_UNABLE_TO_OPEN);

    uint32_t colormap_length = 0;
    uint8_t *colormap = NULL;
    // allocate the colormap and set the corresponding variable if it is an indexed image
    if(img_channels == 1) {
        colormap_length = 256;
        colormap = calloc(colormap_length * 4, sizeof(uint8_t));
        check_memory(colormap) {
            fclose(bmp);
            return ERROR_OUT_OF_MEMORY;
        }
        // fill the colormap with the grayscale color map
        uint32_t rgb = 0;
        for(rgb = 0; rgb < 256; rgb++) {
            colormap[4*rgb + 0] = rgb;
            colormap[4*rgb + 1] = rgb;
            colormap[4*rgb + 2] = rgb;
            colormap[4*rgb + 3] = 0;
        }
    }
    // create a bitmap header
    struct bitmap_header_t header;

    uint32_t row_length = (img_width * img_channels) % 4;
    uint32_t padding = row_length % 4;
    if(padding) {
        padding = (4 - padding);
    }

    // fill the header
    header.bftype[0] = 'B';
    header.bftype[1] = 'M';
    header.bfsize = 54 + colormap_length * 4 + (row_length + padding) * img_height;
    // fill the reserved bytes
    header.bfres1 = 0;
    header.bfres2 = 0;
    header.bfoffset = 54 + colormap_length * 4;
    header.info.bisize = 40;
    header.info.width  = img_width;
    header.info.height = img_height;
    header.info.biplane = 1;
    header.info.bppixel = 8 * img_channels;
    header.info.comtype = 0;
    header.info.rawsize = 0;
    header.info.xpmeter = 0;
    header.info.ypmeter = 0;
    header.info.ncolors = colormap_length;
    header.info.icolors = 0;

    // write the header
    fwrite(header.bftype, sizeof(uint8_t), 2, bmp);
    fwrite(&header.bfsize, sizeof(uint32_t), 1, bmp);
    fwrite(&header.bfres1, sizeof(uint16_t), 1, bmp);
    fwrite(&header.bfres2, sizeof(uint16_t), 1, bmp);
    fwrite(&header.bfoffset, sizeof(uint32_t), 1, bmp);

    // write the info header
    fwrite(&header.info.bisize, sizeof(uint32_t), 1, bmp);
    fwrite(&header.info.width , sizeof(uint32_t), 1, bmp);
    fwrite(&header.info.height, sizeof(uint32_t), 1, bmp);
    fwrite(&header.info.biplane, sizeof(uint16_t), 1, bmp);
    fwrite(&header.info.bppixel, sizeof(uint16_t), 1, bmp);
    fwrite(&header.info.comtype, sizeof(uint32_t), 1, bmp);
    fwrite(&header.info.rawsize, sizeof(uint32_t), 1, bmp);
    fwrite(&header.info.xpmeter, sizeof(uint32_t), 1, bmp);
    fwrite(&header.info.ypmeter, sizeof(uint32_t), 1, bmp);
    fwrite(&header.info.ncolors, sizeof(uint32_t), 1, bmp);
    fwrite(&header.info.icolors, sizeof(uint32_t), 1, bmp);

    // get ready for the reading
    if(header.info.comtype == 0) {
        // read uncompressed bitmap data
        if(header.info.bppixel == 8 || header.info.bppixel == 24) {
            write_bmp_data(bmp, header, data, colormap);
        }
        else {
            fclose(bmp);
            return WARNING_NOT_SUPPORTED;
        }
    } else {
        fclose(bmp);
        return WARNING_NOT_SUPPORTED;
    }
    fclose(bmp);
    // tell that we read the image
    return SUCCESS;
}



