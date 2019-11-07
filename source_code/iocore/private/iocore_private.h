//
// Created by cescript on 1.01.2018.
//

#ifndef IMLAB_IOCORE_PRIVATE_H
#define IMLAB_IOCORE_PRIVATE_H

#include "../iocore.h"
#include "private/core_private.h"


/**@hideinitializer
This function reads the value of #param argument from the given list of string arguments #option and fill the #values pointer.
This method search for the #param in the given string. Any parameter must be followed by ":" and than the double value.
If the parameter is an array than the consequtive numbers must be seperated by "x". After reading a value the function cast the given type to the read value
and fill the #values pointer.
If the given #option is empty or an error occured during parsing, the macro simple returns without an error message.

An example usage of the method is given in the following example:

\code{.c}
int32_t bin = 0;
float blocks[5] = {0};
double cell[2] = {0};

char *options = "bin:9 blocks:1.1x2.2x3.3x4.4x5.5 cell:2x2 stride:1";

getopt(int32_t, options, "bin", &bin);
getopt(float, options, "blocks", blocks);
getopt(double, options, "cell", cell);

printf("bin  : %d\n", bin);
printf("block: %3.2f %3.2f %3.2f %3.2f %3.2f\n", blocks[0], blocks[1], blocks[2], blocks[3], blocks[4]);
printf("cell : %3.2f %3.2f\n", cell[0], cell[1]);

// the above code will print
// bin  : 9
// block: 1.10 2.20 3.30 4.40 5.50
// cell : 2.00 2.00
\endcode

\warning The value pointer must be allocated before calling the function.
\warning Before any casting, this method converts the string into double and than cast the double value into the given type. So there is a chance to be overflow
if the param is 64 bit integer.
*/
#ifndef DOXYGEN_SKIP_IMLAB_HIDDEN_MACRO_NAMES
int32_t getopt_int32_t(char *option, char *param, int32_t *values);
uint32_t getopt_uint32_t(char *option, char *param, uint32_t *values);
uint32_t getopt_float(char *option, char *param, float *values);
uint32_t getopt_double(char *option, char *param, double *values);
#endif

#define getopt(_type, _option, _param, _values) cat(getopt_, _type)(_option, _param, _values)


// read the bmp from the given file
return_t read_bmp(char *filename, uint8_t **data, uint32_t *img_width, uint32_t *img_height, uint32_t *img_channels);
// write the bmp to the given file
return_t write_bmp(char *filename, uint8_t *data, uint32_t img_width, uint32_t img_height, uint32_t img_channels);

#endif //IMLAB_IOCORE_PRIVATE_H

// open the file to read or write. write/read the given key value pair into the file
FILE *imlab_fopen(const char *filename, const char *mode, const char key[64]);

