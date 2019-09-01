/**
\brief Functions for the matrix_t containers
- matrix_create()  : constructor function zero initializer
- matrix_resize()  : change the size of the data holded in container
- matrix_free()    : destructor functions
- matrix_fill()    : set the struct with a constant value
- matrix_load()    : loads the matrix struct from the hard drive (uncompress data)
- matrix_save()    : saves the type struct to the hard drive (compress data)
- matrix_copy()    : creates a copy of the input matrix (just size) and returns it
- matrix_create()   : creates a copy of the input matrix (size and data) and returns it
*/
#include <string.h> // for memcpy
#include <stdlib.h> // for malloc vs
#include "private/core_private.h"


inline struct imlab_type_t *matrix_type(matrix_t *this) { return this->_type; }

void*    mdata    (matrix_t *this, uint32_t idx)  { return (void*)(((int8_t*)this->_data) + this->_type->elemsize * idx); }
uint32_t width    (matrix_t *this)  { return this->_cols; }
uint32_t height   (matrix_t *this)  { return this->_rows; }
uint32_t rows     (matrix_t *this)  { return this->_rows; }
uint32_t cols     (matrix_t *this)  { return this->_cols; }
uint32_t channels (matrix_t *this)  { return this->_channels; }
uint32_t volume   (matrix_t *this)  { return this->_rows*this->_cols*this->_channels; }

/**
Creates a null matrix which does not allocate any memory. This matrix must be resized before actually use it.
@param type Content type of the matrix data.
*/

/**
Creates a new matrix element with the specified paramaters.
@param type Type of the matrix. It could be one of the IMLAB_8U, IMLAB_8S, IMLAB_16U, IMLAB_16S, IMLAB_32U, IMLAB_32S, IMLAB_32F or IMLAB_64F types
@param rows Number of rows (height) of the output matrix.
@param cols Number of columns (width) of the output matrix.
@param channels Number of channels of the output matrix.
*/


// calls the actual function with the appropriate way
// actual function which does the job
matrix_t *matrix_create_(struct imlab_type_t *_type, uint32_t _rows, uint32_t _cols, uint32_t _channels, void *ptr, uint8_t use_pointer) {
    matrix_t *this = malloc(sizeof(matrix_t));
    this->_type = _type;
    // first try to get the memory or use the given memory
    if(use_pointer == 0) {
        this->_data = calloc(_rows*_cols*_channels, _type->elemsize);
    } else {
        this->_data = ptr;
    }
    // if the matrix is not allocate memory, print an error and return an empty matrix
    if(this->_data == NULL) {
        this->_rows = 0;
        this->_cols = 0;
        this->_channels = 0;
    } else {
        this->_rows = _rows;
        this->_cols = _cols;
        this->_channels = _channels;
    }
    // if any pointer is given for copy, copy it
    if(use_pointer == 0 && ptr != NULL) {
        memcpy(this->_data, ptr, this->_rows*this->_cols*this->_channels * _type->elemsize);
    }
    // the matrix is ready to return
    return this;
}

// create a null matrix
matrix_t* matrix_null() {
    return NULL;
}

/**
Sets the (r,c,k)'th element of the matrix to the given value when the type of the input matrix is unknown. Use at() to get/set an element of the matrix if you know the type.
This function is intented to be used internally so in order to be fast it doesnt control the idx and it will create a buffer overflow if index is larger than the volume of the matrix.
@param var Input matrix.
@param rows Row of the matrix element to be set.
@param cols Coloumn of the matrix element to be set.
@param channels Channel of the matrix element to be set.
@param value Address of the data to be set.
*/
void matrix_set(matrix_t *var, uint32_t _rows, uint32_t _cols, uint32_t _channels, void *value) 
{
    void *_data = mdata(var, idx(var, _rows, _cols, _channels));
    memcpy(_data, value, elemsize(var));
}

/**
Gets the (r,c,k)'th element of the matrix to the given variable when the type of the input matrix is unknown. Use at() to get/set an element of the matrix if you know the type.
This function is intented to be used internally so in order to be fast it doesnt control the idx and it will create a buffer overflow if index is larger than the volume of the matrix.
@param var Input matrix.
@param rows Row of the matrix element to be set.
@param cols Coloumn of the matrix element to be set.
@param channels Channel of the matrix element to be set.
@param value Address of the data to be get.
*/
void matrix_get(matrix_t *var, uint32_t _rows, uint32_t _cols, uint32_t _channels, void *value) 
{
    void *_data = mdata(var, idx(var, _rows, _cols, _channels));
    memcpy(value, _data, elemsize(var));
}

/**
Resize the input matrix to the specified size. This function is used most of the functions in order to create output matrix.
@param _var Input matrix to be resized.
@param _nrows New rows of the input matrix.
@param _ncols New columns of the input matrix.
@param _nchannels New channels of the input matrix.
*/
return_t matrix_resize(matrix_t *var, uint32_t _rows, uint32_t _cols, uint32_t _channels) 
{
    // check the matrix is valid, otherwise return error
    check_null(var, ERROR_NULL_TYPE);
    // if the sizes are the same, do nothing
    if(var->_rows == _rows && var->_cols == _cols && var->_channels == _channels) {
        return WARNING_NOTHING_DONE;
    }
    // try to get a space for the resized matrix
    void *tmp = realloc(var->_data, elemsize(var) * _rows*_cols*_channels);
    // if you get a space resize the input, otherwise do nothing
    check_memory(tmp, ERROR_OUT_OF_MEMORY);
    // if memory is ok, set the new values
    var->_rows = _rows;
    var->_cols = _cols;
    var->_channels = _channels;
    var->_data = tmp;
    // the matrix is ready to returnt
    return SUCCESS;
}

/**
 * Sets a destructor for the data holded in the matrix container
 * @param in Input matrix
 * @param func Destructor function to be set
 */
void matrix_destructor(matrix_t *in, void (*func)(void*, uint32_t)) 
{
    imlab_type_set_destructor(in->_type, func);
}

/**
Release the allocated memory for the matrix object.
@param _var Input matrix to be freed.
*/
void matrix_free(matrix_t **var) 
{
    // destruct each element one by one
    if(!(var[0] == NULL)) {
        if(var[0]->_type->destructor != NULL) {
            var[0]->_type->destructor(var[0]->_data, volume(var[0]));
        }
        imlab_type_free(&var[0]->_type);
    }
    if(var[0]->_data != NULL) {
        free(var[0]->_data);
    }
    free(var[0]);
}

/**
Fill a matrix with the desired value. If there are multiple channels(channels != 1) than instead of sending
single variable, send array of data which length is equal to channels
// double array[] = {255,0,128};
// matrix_fill(color_image, array)
// by this way, _fill will copy 255 to the first, 0 to second and 128 to the
// third channel of the image
@param _var Input matrix to be filled.
@param _value Data to be written to each cell of the input matrix.
*/
return_t matrix_fill(matrix_t *out, void *value)
{

    check_null(out, ERROR_NULL_TYPE);
    // row column and channel indices
    size_t r;
    uint8_t *out_data = (uint8_t*)out->_data;
    // necessary bytes for a matrix element (channels*elementsize)
    size_t step_size = out->_channels*elemsize(out);
    for(r = 0; r <  out->_rows*out->_cols; r++) {
        memcpy(out_data, value, step_size);
        out_data += step_size;
    }
    return SUCCESS;
}

// copy source matrix into the destination matrix, resize the destination if needed
return_t matrix_copy(matrix_t *src, matrix_t *dst) 
{
    if(!is_sametype(src, dst)) {
        message(ERROR, "cannot copy matrices which have different types!");
        return ERROR_TYPE_MISMATCH;
    }
    // start copying matrices
    matrix_resize(dst, src->_rows, src->_cols, src->_channels);
    memcpy( dst->_data, src->_data, elemsize(src) * volume(src));
    return SUCCESS;
}

// save the the given matrix_t structure into the file
return_t matrix_write(matrix_t *src, const char *filename) 
{

    check_null(src, ERROR_NULL_TYPE);
    // load wrt file version
    FILE *fout = fopen(filename, "wb");
    check_file(fout, ERROR_UNABLE_TO_OPEN);
    // write the type information
    imlab_type_write(src->_type, fout);
    // typedef typedef struct vector_ { struct imlab_type_t type; uint32_t capacity, length; void *data; }     vector_t;
    fwrite(&src->_rows, sizeof(uint32_t), 1, fout);
    fwrite(&src->_cols, sizeof(uint32_t), 1, fout);
    fwrite(&src->_channels, sizeof(uint32_t), 1, fout);
    // write the data
    fwrite(src->_data, elemsize(src), volume(src), fout);
    // end of file
    fclose(fout);
    return SUCCESS;
}

// load the file into the givenvector_t structure
matrix_t* matrix_read(const char *filename) 
{
    // load wrt file version
    FILE *fin = fopen(filename, "rb");
    check_file(fin, NULL);
    // read the type information
    struct imlab_type_t* matrix_type = imlab_type_read(fin);
    // typedef typedef struct vector_ { struct imlab_type_t type; uint32_t capacity, length; void *data; }     vector_t;
    uint32_t matrix_rows, matrix_cols, matrix_channels = 0;
    fread(&matrix_rows, sizeof(uint32_t), 1, fin);
    fread(&matrix_cols, sizeof(uint32_t), 1, fin);
    fread(&matrix_channels, sizeof(uint32_t), 1, fin);
    // allocate memory for the matrix
    matrix_t *out = matrix_create_(matrix_type, matrix_rows, matrix_cols, matrix_channels, NULL, 0);
    // read the data
    fread(out->_data, elemsize(out), volume(out), fin);
    // end of file
    fclose(fin);
    return out;
}

// reshape the matrix into 1xN or Nx1 dimensional vector
vector_t* matrix2vector(matrix_t *input, uint8_t order) 
{

    vector_t *out = vector_create_(typeof(input), 1, NULL);

    uint32_t r, c, k;

    uint8_t *input_data = data(uint8_t, input);
    // row first order
    if(order == 0) {
        for(r = 0; r < rows(input); r++) {
            for(c = 0; c < cols(input); c++) {
                for(k = 0; k < channels(input); k++) {
                    vector_push(out, input_data + elemidx(input, r,c));
                }
            }
        }
    }
    else if(order == 1) {
        for(c = 0; c < cols(input); c++) {
            for(r = 0; r < rows(input); r++) {
                for(k = 0; k < channels(input); k++) {
                    vector_push(out, input_data + elemidx(input, r,c));
                }
            }
        }
    }
    // return the output vector
    return out;
}

// print the properties of the matrix
void matrix_view(matrix_t *in) 
{
    // cannot print anything if the matrix is null
    check_null(in, );

    printf("Parameters of the Matrix\n");
    printf("Type    : %s (%d bytes)\n", imlab_type_name(in->_type), imlab_type_elemsize(in->_type));

    printf("Width   : %d\n", in->_cols);
    printf("Height  : %d\n", in->_rows);
    printf("Channels: %d\n", in->_channels);
    printf("Volume  : %d\n", volume(in));

    // bytes allocated for the matrix
    size_t total_size = sizeof(matrix_t);
    if(in->_type) {
        total_size += strlen(in->_type->name);
        total_size += sizeof(struct imlab_type_t);
        total_size += volume(in) * elemsize(in);
    }

    // pretty print the memory size
    if(total_size > 1024*1024*1024) {
        printf("Memory Size  : %3.2f GB\n", total_size / (float)(1024*1024*1024));
    } else if(total_size > 1024*1024) {
        printf("Memory Size  : %3.2f MB\n", total_size / (float)(1024*1024));
    } else if(total_size > 1024) {
        printf("Memory Size  : %3.2f kB\n", total_size / (float)(1024));
    } else {
        printf("Memory Size  : %d bytes\n", total_size);
    }


}