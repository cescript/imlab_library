#include <string.h>
#include <stdlib.h>
#include "private/core_private.h"

/**
\brief Functions for the vector_t containers
- vector_create()  : constructor function and zero initializer
- vector_resize()  : change the size of the data holded in container
- vector_free()    : destructor functions
- vector_fill()    : set the struct with a constant value
- vector_permute() : permute the entries of the vector with the given index list
- vector_push()    : push an instance to the vector, if there is no space, creates one
- vector_pop()     : get an index from the vector and remove it from the array
- vector_load()    : loads the vector struct from the hard drive (uncompress data)
- vector_save()    : saves the type struct to the hard drive (compress data)
*/

//typedef struct _vector { struct imlab_type_t _type; uint32_t _capacity, _length; void *_data; }     vector_t;

struct imlab_type_t *vector_type(vector_t *this) { return this->_type; }

void*    vdata    (vector_t *this, uint32_t idx)  { return (void*)(((int8_t*)this->_data) + this->_type->elemsize * idx); }
uint32_t length   (vector_t *this) { return this->_length; }
uint32_t capacity (vector_t *this) { return this->_capacity; }

/**
Create a new vector with the specified input paramaters.
@param type Content type of the vector data. It could be any type char,int, float or user defined structs.
@param capacity Capacity of the output vector. If you dont know the exact size of the
vector at the programming stage just write the expected length of the vector. IMLAB automatically
increase the size if the data exceeds the vector length (This is only true if you use vector_push for the data insertion).
@param pointer A pointer to the vector data. If this parameter is send to the function imlab will not allocate a new
memory and uses the send pointer as the container.
*/

// this is the real allocater function which is called by a vector_create macro
vector_t *vector_create_(struct imlab_type_t *_type, uint32_t _capacity, void *ptr) {
    vector_t *this = malloc(sizeof(vector_t));
    // assign values to the new vector
    this->_type = _type;
    // try to allocate pointer
    this->_data     = calloc(_capacity , _type->elemsize);
    if(this->_data == NULL) {
        this->_capacity = 0;
        this->_length   = 0;
    } else {
        this->_capacity = _capacity;
        this->_length   = 0;
    }
    // copy the given pointer into the new vector
    if(ptr != NULL) {
        memcpy(this->_data, ptr, _capacity * _type->elemsize);
        this->_length = _capacity;
    }
    // we allocate a new vector, now we can return it
    return this;
}

// create a NULL vector and return it
vector_t* vector_null() {
    // return it
    return NULL;
}

/**
Sets the idx'th element of the vector to the given value when the vector type is unknown. This is not the correct way to insert an element to the vector.
Use vector_push for adding a new element to the vector. If you want to just set an element of the vector use at(type, var, idx) = value.
This function is intented to be used internally or in loops so in order to be fast it doesnt control the idx and it will create a serious bug
if you try to insert an element at any index larger than the length of the vector and it will create a buffer overflow if index is larger than the capacity of the vector.
@param var Input vector.
@param idx Index of the vector element to be set.
@param value Address of the data to be set.
*/
void vector_set(vector_t *var, uint32_t _idx, void *value) {
    void *var_data = vdata(var, _idx);
    memcpy(var_data, value, elemsize(var));
}

/**
Gets the idx'th element of the vector to the given variable. This is not the correct way to extract an element from the vector.
Use vector_pop for extracting an element from the vector. If you want to just access an element of the vector use value = at(type, var, idx).
This function is intented to be used internally or in loops so in order to be fast it doesnt control the idx and it will create a serious bug
if you try to get an element at any index larger than the length of the vector and it will create a buffer overflow if index is larger than the capacity of the vector.
@param var Input vector.
@param idx Index of the vector element to be set.
@param value Address of the data to be set.
*/
void vector_get(vector_t *var, uint32_t _idx, void *value) {
    uint8_t *var_data = vdata(var, _idx);
    memcpy(value, var_data, elemsize(var));
}


// vector resize the vector to its length so the unused areas are deleted
return_t vector_resize(vector_t *var, uint32_t _capacity) {

    check_null(var, ERROR_NULL_TYPE);
    // get the current capacity
    uint32_t old_capacity = var->_capacity;
    if(_capacity == 0) {
        _capacity = min_2(1, var->_length);
    }
    void *tmp = realloc(var->_data, elemsize(var) * _capacity);
    // did you allocate the memory?
    check_memory(tmp);

    // if you get it, than set the new limits
    var->_capacity = _capacity;
    var->_data = tmp;
    // return ok
    return SUCCESS;
}

/**
 * Sets a destructor for the data holded in the vector container
 * @param in Input vector
 * @param func Destructor function to be set
 */
void vector_destructor(vector_t *in, void (*func)(void*, uint32_t)) {
    imlab_type_set_destructor(in->_type, func);
}

/**
Release the allocated memory for the vector object.
@param _var Input vector to be freed.
*/
void vector_free(vector_t **var) {
    // destruct each element one by one
    if(! (var[0] == NULL)) {
        if(var[0]->_type->destructor != NULL) {
            var[0]->_type->destructor(var[0]->_data, length(var[0]));
        }
        // destruct the remaining variables
        imlab_type_free(&var[0]->_type);
    }
    if(var[0]->_data != NULL) {
        free(var[0]->_data);
    }
    free(var[0]);
}

/**
Release the allocated memory for the vector object.
@param _var Input vector to be freed.
*/
void vector_reset(vector_t **var)
{
    // destruct each element one by one
    if (!(var[0] == NULL))
    {
        if (var[0]->_type->destructor != NULL)
        {
            var[0]->_type->destructor(var[0]->_data, length(var[0]));
        }
        // destruct the remaining variables
        imlab_type_free(&var[0]->_type);
    }

    // free previous memory
    if (var[0]->_data != NULL)
    {
        free(var[0]->_data);
    }

    // try to reallocate the memory
    var[0]->_data = calloc(1, var[0]->_type->elemsize);

    if (var[0]->_data == NULL)
    {
        var[0]->_capacity = 0;
        var[0]->_length = 0;
    }
    else
    {
        var[0]->_capacity = 1;
        var[0]->_length = 0;
    }
}

/**
Fill a vector with the desired value.
@param _var Input vector to be filled.
@param _value Data to be written to each cell of the input vector.
*/
return_t vector_fill(vector_t *var, void *value) {
    uint32_t i = 0;
    // fill the all elements of the vector
    size_t step_size = elemsize(var);
    uint8_t *var_data = data(uint8_t, var, 0);
    for(i = 0; i < var->_capacity; i++) {
        memcpy(var_data, value, step_size);
        var_data += step_size;
    }
    var->_length = var->_capacity;
    return SUCCESS;
}

/**
Permute the entries of the vector with the given index list.
@param _var Input vector to be permuted.
@param _idx Index list for the permuted vector.
*/
return_t vector_permute(vector_t *var, uint32_t *index_list) {

    uint32_t i,l, new_idx;
    uint8_t *in_data = data(uint8_t, var);
    uint8_t *in_copy = (uint8_t*) malloc(var->_length * elemsize(var));
    // did you allocate spave?
    check_memory(in_copy);
    // get a copy of the input
    memcpy(in_copy, in_data, var->_length*elemsize(var));

    for(i=0; i < var->_length; i++) {
        new_idx = index_list[i];
        // use memcpy or for loop, select one
        for(l=0; l < elemsize(var); l++) {
            in_data[l] = in_copy[l + elemsize(var)*new_idx];
        }
        in_data += elemsize(var);
    }
    // remove unnecessary array
    free(in_copy);
    // return ok
    return SUCCESS;
}

// actual push function
return_t vector_push(vector_t *var, void *element) {

    check_null(var, ERROR_NULL_TYPE);
    // try to allocate memory if the vector is full
    if( length(var) == capacity(var) ) {
        return_t res = vector_resize(var, var->_length << 1);
        if(is_error(res)) {
            return res;
        }
    }
    // vector size is ready for the next element, push it
    vector_set(var, var->_length++, element);
    // return success
    return SUCCESS;
}

/**
Pops an element from the end of the vector and deletes the last element of the vector.
This method does not change the capacity of the vector. You can manually resize the vector
if you want to use less memory.
@param _var Input vector.
@param _element Address of an element to be popped into.
*/
// actual pop function
return_t vector_pop(vector_t *var, void *element) {

    check_null(var, ERROR_NULL_TYPE);
    if(var->_length > 0) {
        vector_get(var, --var->_length, element);
    } else {
        message(ERROR, "no data left in the vector");
        return ERROR;
    }
    return SUCCESS;
}


// save the the given matrix_t structure into the file
return_t vector_write(vector_t *src, const char *filename) {

    check_null(src, ERROR_NULL_TYPE);
    // load wrt file version
    FILE *fout = fopen(filename, "wb");
    check_file(fout, ERROR_UNABLE_TO_OPEN);
    // write the type information
    imlab_type_write(src->_type, fout);
    // typedef typedef struct vector_ { struct imlab_type_t type; uint32_t capacity, length; void *data; }     vector_t;
    fwrite(&src->_capacity, sizeof(uint32_t), 1, fout);
    // read the length of the vector
    fwrite(&src->_length,  sizeof(uint32_t), 1, fout);
    // read the data
    fwrite(src->_data, elemsize(src), capacity(src), fout);
    // end of file
    fclose(fout);
    return SUCCESS;
}

// load the file into the givenvector_t structure
vector_t* vector_read(const char *filename) {
    // load wrt file version
    FILE *fin = fopen(filename, "rb");
    check_file(fin, vector_null());

    // read the type information
    struct imlab_type_t *vector_type = imlab_type_read(fin);
    // typedef typedef struct vector_ { struct imlab_type_t type; uint32_t capacity, length; void *data; }     vector_t;
    uint32_t vector_capacity = 0;
    fread(&vector_capacity, sizeof(uint32_t), 1, fin);
    // allocate memory for the vector
    vector_t* out = vector_create_(vector_type, vector_capacity, NULL);
    // read the length of the vector
    fread(&out->_length,  sizeof(uint32_t), 1, fin);
    // read the data
    fread(out->_data, elemsize(out), capacity(out), fin);
    // end of file
    fclose(fin);
    return out;
}

// return the unique elements of the input vector
return_t vector_unique(vector_t *src, vector_t *uniques, vector_t *unique_idx) {
    check_null(src, ERROR_NULL_TYPE);
    check_condition(is_numeric(src) || is_integer(src), ERROR_TYPE_MISMATCH, "input must be number");
    check_condition(typeid(src) == typeid(uniques), ERROR_TYPE_MISMATCH, "input and output types must be same");
    // create an typeless function
    #define find_uniquess(_src_type, _src_, _uniques_, _unique_idx_) do {\
        uint32_t _i, _j,_idx = 0;\
        _src_type *_src_data = data(_src_type, _src_);\
        vector_push(_uniques_, &_src_data[0]);\
        if(unique_idx) { vector_push(_unique_idx_, &_idx); }\
        for(_i = 1; _i < length(_src_); _i++) {\
            uint8_t _is_unique = 1;\
            for(_j = 0; _j < length(_uniques_) & _is_unique; _j++) {\
                if(at(_src_type, _uniques_, _j) == _src_data[_i]) {\
                    _is_unique = 0;\
                }\
            }\
            if(_is_unique) {\
                vector_push(_uniques_, &_src_data[_i]);\
                _idx++;\
            }\
            if(unique_idx) { vector_push(_unique_idx_, &_idx); }\
        }\
    } while(0)\
    // compare the elements wrt types
    if(is_8s(src) || is_8u(src)) {
        find_uniquess(uint8_t, src, uniques, unique_idx);
    }
    else if(is_16s(src) || is_16u(src)) {
        find_uniquess(uint16_t, src, uniques, unique_idx);
    }
    else if(is_32s(src) || is_32u(src)) {
        find_uniquess(uint32_t, src, uniques, unique_idx);
    }
    else if(is_32f(src)) {
        find_uniquess(float, src, uniques, unique_idx);
    }
    else if(is_64f(src)) {
        find_uniquess(double, src, uniques, unique_idx);
    }
    // delete the temporary function
    #undef find_uniquess
    // return success
    return SUCCESS;
}

// print the properties of the vector
void vector_view(vector_t *in) {
    // cannot print anything if the vector is null
    check_null(in, );

    printf("Parameters of the Vector\n");

    printf("Type     : %s (%d bytes)\n", in->_type->name, in->_type->elemsize);
    printf("Length   : %d\n", in->_length);
    printf("Capacity : %d\n", in->_capacity);

    // bytes allocated for the vector
    size_t total_size = sizeof(vector_t);
    if(in->_type) {
        total_size += strlen(in->_type->name);
        total_size += sizeof(struct imlab_type_t);
        total_size += in->_capacity * elemsize(in);
    }
    // pretty print the memory size
    if(total_size > 1024*1024*1024) {
        printf("Memory Size  : %3.2f GB\n", total_size / (float)(1024*1024*1024));
    } else if(total_size > 1024*1024) {
        printf("Memory Size  : %3.2f MB\n", total_size / (float)(1024*1024));
    } else if(total_size > 1024) {
        printf("Memory Size  : %3.2f kB\n", total_size / (float)(1024));
    } else {
        printf("Memory Size  : %zu bytes\n", total_size);
    }
}
