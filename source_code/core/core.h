/**
 * @file core.h
 * @author My Self
 * @date 9 Sep 2012
 * @brief File containing example of doxygen usage for quick reference.
 *
 * Here typically goes a more extensive explanation of what the header
 * defines. Doxygens tags are words preceeded by either a backslash @\
 * or by an at symbol @@.
 * @see http://www.stack.nl/~dimitri/doxygen/docblocks.html
 * @see http://www.stack.nl/~dimitri/doxygen/commands.html
 */
////////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For IMLAB Computer Vision Library
//
// Copyright (C) 2016, Image and Matrix Laboratory Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
// Last Update: 08-05-2017

#ifndef __IMLAB_CORE_H__
#define __IMLAB_CORE_H__

#include <stdio.h> // only for print error
#include <stdint.h>
#include <stdarg.h> // only for string_printf
#include <string.h> // only for getopt strlen
#include "core_macros.h"


/* IMLAB SETTINGS FOR THE WHOLE LIBRARY */
/* USED IN IMLAB_TYPES */
// Color terminal output for the IMLAB library
#define IM_VERBOSE_ERROR       1
#define IM_VERBOSE_WARNING     1
#define IM_VERBOSE_SUCCESS     1
#define IMLAB_USE_OPENMP 1
/* END OF SETTINGS */
//

/* GLOBAL IMLAB TYPE IDENTIFIERS */
#ifndef DOXYGEN_SKIP_IMLAB_HIDDEN_MACRO_NAMES
#define IMLAB_TYPE_NULL       0
// generic types for the c
#define IMLAB_TYPE_8S         1
#define IMLAB_TYPE_8U         2
#define IMLAB_TYPE_16S        3
#define IMLAB_TYPE_16U        4
#define IMLAB_TYPE_32S        5
#define IMLAB_TYPE_32U        6
#define IMLAB_TYPE_32F        7
#define IMLAB_TYPE_64F        8
// additional types for the IMLAB
#define IMLAB_TYPE_POINT      9
#define IMLAB_TYPE_RECTANGLE 10
#define IMLAB_TYPE_STRING    11
#define IMLAB_TYPE_JSON_OBJECT 12
#define IMLAB_TYPE_JSON_ARRAY  13
#define IMLAB_TYPE_VALID     14
#endif
/* END OF IMLAB GLOBAL MACROS */


/* IMLAB PUBLIC TYPE IDENTIFIER */
struct imlab_type_t;

/**@hideinitializer
 * @brief Return the index of the data at the given position.
 *
\code{.c}
matrix_t *matA = matrix_create(float, 5,5);
matrix_t *vecA = vector_create(uint16_t, 5);

uint32_t i1 = idx(matA, 0,1); // 1
uint32_t i2 = idx(matA, 0,2); // 2
uint32_t i3 = idx(matA, 1,1); // 6
uint32_t i4 = idx(matA, 1,2); // 7

uint32_t i5 = idx(vecA, 1); // 1
uint32_t i6 = idx(vecA, 2); // 2
\endcode
 */
#define idx(...) call(idx, __VA_ARGS__)
#ifndef DOXYGEN_SKIP_IMLAB_HIDDEN_MACRO_NAMES
#define idx_1(_var) (0)
#define idx_2(_var, _c) (_c)
#define idx_3(_var, _r,_c) (channels(_var)*((_c) + cols(_var)*(_r)))
#define idx_4(_var, _r,_c,_k) (channels(_var)*((_c) + cols(_var)*(_r)) + (_k))
#endif

/**@hideinitializer
 * @brief Return the index of the given position in a byte array
\code{.c}
matrix_t *matA = matrix_create(float, 5,5);
matrix_t *vecA = vector_create(uint16_t, 5);

uint32_t i1 = elemidx(matA, 0,1); // 4
uint32_t i2 = elemidx(matA, 0,2); // 8
uint32_t i3 = elemidx(matA, 1,1); // 24
uint32_t i4 = elemidx(matA, 1,2); // 28

uint32_t i5 = elemidx(vecA, 1); // 2
uint32_t i6 = elemidx(vecA, 2); // 4
\endcode
 */
#define elemidx(...) (elemsize(arg_1(__VA_ARGS__)) * idx(__VA_ARGS__))
/**@hideinitializer
 * @brief Return the size of the element hold in the data pointer
\code{.c}
matrix_t *matA = matrix_create(float, 5,5);
matrix_t *vecA = vector_create(uint16_t, 5);
// get the element size of the container data
uint32_t i1 = elemsize(matA); // 4
uint32_t i2 = elemsize(vecA); // 2
\endcode
 */
#define elemsize(_var) imlab_type_elemsize((_var)->_type)

/**
@hideinitializer
@brief Returns the data value at the given position for the given imlab object.

It is overloaded with the number of input arguments.
In the simplest case data takes two argument which is the type and imlab struct and returns the dataa at the zero index of the given variable.
@code{.c}
matrix_t *matf = matrix_create(float, 100, 100);
float mdata = at(float, matf); // mdata points to matf->_data(0,0,0)
float mdata = atf(matf); // mdata points to matf->_data(0,0,0)
@endcode
#at macro can also be called with a starting index as follows:
@code{.c}
vector_t veci = vector_create(uint32_t, 100);
uint32_t vdata = at(uint32_t, veci, 5); // vdata points to the veci->_data(5)
uint32_t vdata = atui32(veci, 5); // vdata points to the veci->_data(5)
@endcode
This macro can also be used to access a specific position of a matrix:
@code{.c}
matrix_t mati = matrix_create(uint32_t, 100, 100, 3);
uint32_t m1data = at(uint32_t, mati, 5); // m1data points to the mati->_data(0,5,0)
uint32_t m2data = at(uint32_t, mati, 5, 7); // m2data points to the mati->_data(5,7,0)
uint32_t m3data = at(uint32_t, mati, 5, 7, 2); // m3data points to the mati->_data(5,7,2)
@endcode
**/
#define at(_type, ...) ( ((_type *)(arg_1(__VA_ARGS__)->_data))[idx(__VA_ARGS__)] )

#ifndef DOXYGEN_SKIP_IMLAB_HIDDEN_MACRO_NAMES

#define atf(...) (((float *)(arg_1(__VA_ARGS__)->_data))[idx(__VA_ARGS__)])
#define atd(...) (((double *)(arg_1(__VA_ARGS__)->_data))[idx(__VA_ARGS__)])

#define ati8(...) (((int8_t *)(arg_1(__VA_ARGS__)->_data))[idx(__VA_ARGS__)])
#define atui8(...) (((uint8_t *)(arg_1(__VA_ARGS__)->_data))[idx(__VA_ARGS__)])

#define ati16(...) (((int16_t *)(arg_1(__VA_ARGS__)->_data))[idx(__VA_ARGS__)])
#define atui16(...) (((uint16_t *)(arg_1(__VA_ARGS__)->_data))[idx(__VA_ARGS__)])

#define ati32(...) (((int32_t *)(arg_1(__VA_ARGS__)->_data))[idx(__VA_ARGS__)])
#define atui32(...) (((uint32_t *)(arg_1(__VA_ARGS__)->_data))[idx(__VA_ARGS__)])

#endif

// get the data pointer of the matrix/vector
/**
@hideinitializer
@brief Returns the data pointer for the given imlab container.

It is overloaded with the number of input arguments.
In the simplest case data takes two argument which are the type and imlab struct and returns the pointer to the data.
@code{.c}
matrix_t matf = matrix_create(float, 100, 100);
float *matf_data = data(float, matf); // mdata points to &(matf->_data(0,0,0))
@endcode
#data macro can also be called with a starting index as follows:
@code{.c}
vector_t veci = vector_create(uint32_t, 100);
uint32_t *v1data = data(uint32_t, veci, 5); // v1data points to the &(veci->_data(5))
@endcode
This macro can also be used to access a specific position of a matrix:
@code{.c}
matrix_t mati = matrix_create(uint32_t, 100, 100, 3);
uint32_t *m1data = data(uint32_t, mati, 5); // m1data points to the &(mati->_data(0,5,0))
uint32_t *m2data = data(uint32_t, mati, 5, 7); // m2data points to the &(mati->_data(5,7,0))
uint32_t *m3data = data(uint32_t, mati, 5, 7, 2); // m3data points to the &(mati->_data(5,7,2))
@endcode
**/
#define data(_type, ...) ( ((_type *)(arg_1(__VA_ARGS__)->_data)) + idx(__VA_ARGS__) )


/**\name  IMLAB Type Comparators
 * Returns true if the asked question is true for matrix/vector
 *  @{
 */
/// @hideinitializer return true if the given matrix or vector holds NULL data type
// #define is_null(var)  (typeid(var) == IMLAB_TYPE_NULL)
/// @hideinitializer return true if the given matrix or vector holds int8_t data type
#define is_8s(var)    (typeid(var) == IMLAB_TYPE_8S )
/// @hideinitializer return true if the given matrix or vector holds uint8_t data type
#define is_8u(var)    (typeid(var) == IMLAB_TYPE_8U )
/// @hideinitializer return true if the given matrix or vector holds int16_t data type
#define is_16s(var)   (typeid(var) == IMLAB_TYPE_16S)
/// @hideinitializer return true if the given matrix or vector holds uint16_t data type
#define is_16u(var)   (typeid(var) == IMLAB_TYPE_16U)
/// @hideinitializer return true if the given matrix or vector holds int32_t data type
#define is_32s(var)   (typeid(var) == IMLAB_TYPE_32S)
/// @hideinitializer return true if the given matrix or vector holds uint32_t data type
#define is_32u(var)   (typeid(var) == IMLAB_TYPE_32U)
/// @hideinitializer return true if the given matrix or vector holds float data type
#define is_32f(var)   (typeid(var) == IMLAB_TYPE_32F)
/// @hideinitializer return true if the given matrix or vector holds double data type
#define is_64f(var)   (typeid(var) == IMLAB_TYPE_64F)
/// @hideinitializer return true if the given matrix or vector holds a JSON array
#define is_json_array(var) (typeid(var) == IMLAB_TYPE_JSON_ARRAY)
/// @hideinitializer return true if the given matrix or vector holds a JSON object
#define is_json_object(var) (typeid(var) == IMLAB_TYPE_JSON_OBJECT)
/// @hideinitializer return true if the given matrix or vector is image
#define is_image(var) ( var == NULL ? 0 : (is_8u(var) || is_32f(var)) )
/// @hideinitializer return true if the given matrix or vector is integer
#define is_integer(var) ( (is_8s(var) || is_8u(var) || is_16s(var) || is_16u(var) || is_32s(var) || is_32u(var)))
/// @hideinitializer return true if the given matrix or vector is floating point number
#define is_numeric(var) (is_32f(var) || is_64f(var))
/// @hideinitializer return true if the given two matrice or vectors have the same type
#define is_sametype(var1, var2) (typeid(var1) == typeid(var2) && typeid(var1) != IMLAB_TYPE_NULL)

/** @} */

/**@hideinitializer
 * @brief Return the type information of the given IMLAB container
 */
#define typeof(_var) ((_var)->_type)
/**@hideinitializer
 * @brief Return the type id of the given IMLAB container
 */
#define typeid(_var) (imlab_type_id(typeof(_var)))

/**@hideinitializer
 * @brief Return the type of the given container as string
 */
#define typename(_var) (imlab_type_name(typeof(_var)))

/**@hideinitializer
 * @brief Create a new IMLAB container compatible type from the given type name
 */
#define type(type_name) imlab_type_create(#type_name, sizeof(type_name))

#ifndef DOXYGEN_SKIP_IMLAB_HIDDEN_MACRO_NAMES
// returns the ID of the given type
uint8_t imlab_type_id(struct imlab_type_t *tinfo);
// returns the element size of the given type
uint16_t imlab_type_elemsize(struct imlab_type_t *tinfo);
// returns the type name of the givven type as string
char* imlab_type_name(struct imlab_type_t *tinfo);
// clears the memory hold by the given type
void imlab_type_free(struct imlab_type_t **var);
// creates a copy of the input type
struct imlab_type_t *imlab_type_copy(struct imlab_type_t *type);
// creates an IMLAb type with the given string type name and element size
struct imlab_type_t *imlab_type_create(char *type_name, uint32_t type_size);
#endif



/* STRUCTURES TYPEDEFED BY IMLAB LIBRARY */

/* IMLAB PUBLIC RETURN TYPE */
/**
 * @brief IMLAB return type
 * Return type for most of the IMLAB function. Returns a negative integer when an error occurs. Than this error can be categorized using the value of the return.
 * Return a positive integer when everything done without any problem.
 */
typedef enum return_ {
    ERROR_UNABLE_TO_READ = -8,
    ERROR_NOT_IMAGE = -7,
    ERROR_NULL_TYPE = -6,
    ERROR_TYPE_MISMATCH = -5,
    ERROR_DIMENSION_MISMATCH = -4,
    ERROR_OUT_OF_MEMORY  = -3,
    ERROR_UNABLE_TO_OPEN = -2,
    ERROR = -1,
    // SUCCESS NEUTRAL
    SUCCESS = 0,
    // WARNING POSITIVE
    WARNING = 1,
    WARNING_NOT_SUPPORTED = 2,
    WARNING_NOTHING_DONE = 3,

} return_t;

/**
@hideinitializer
@brief Print colored and formatted debug message into the stderr. This function called via message macro.
*/
int print_message_func(return_t cond, int line, const char *func, const char *fmt, ...);

#define message(cond, ...) print_message_func(cond, __LINE__, __func__, __VA_ARGS__)


/**
@hideinitializer
@brief Create a new array with the specified input paramaters.
This macro definition calls the array_create function with the correct values of arguments.
This macro overloaded with respect to number of arguments. The actual function is implemented to support one to four input arguments.
The macro implementation could take up to five arguments as follows:
\verbatim
array_create(<type>, dim1length, dim2length = 0, dim3length = 0, dim4length = 0)
@endverbatim
In the simplest case the function could take the only type of the array and length of the array.
This type could be any c types defined in stdint.h or struct created by the user.
@code{.c}
uint32_t i;
uint32_t *out = array_create(uint32_t, 256);
for(i = 0; i < 256; i++) {
    out[i] = i * 3;
}
array_free(out);
@endcode
The above code will create an array object with the given length and initialize it with zeros. Than it can be used just as regular c pointers.
The only important remark is to free the pointer allocated by #array_create using the #array_free.
If you want to create multi dimensional array, call the function as follows:
@code{.c}
float **out = array_create(float, 1024, 100);
for(i = 0; i < array_size(out,0); i++) {
    for(j = 0; j < array_size(out,1); j++) {
        out[i][j] = (i * j) / 1024;
    }
}
array_free(out);
@endcode
This call of the #array_create will create a 2 dimensional floating point data pointer and allocate 1024*100*sizeof(float) byte memory on the memory.
It can be used as regular two dimensional arrays.

@param <type> Content type of the array data. It could be any type char,uint32_t, float or user defined structures.
@param dimXlength Length of the output array in the Xth dimension.
*/
#define array_create(...) call(array_create, __VA_ARGS__)

/**
 * Return the size of the array in the given dimension. If the array is NULL or dim is higher than 4, this function returns 0.
 * @param head Input array allocated by #array_create
 * @param dim Dimension of the input array
 * @return Length of the array in the given dimension
 */
uint32_t array_size(void *head, uint32_t dim);

/**@hideinitializer
 * @brief Free the memory allocated by the given array and set the pointer to NULL
 * @param head Input array to be deleted.
 * \remark Input array must be allocated by #array_create
 */
void array_free(void *head);


// not include these macros in the documentation
#ifndef DOXYGEN_SKIP_IMLAB_HIDDEN_MACRO_NAMES

#define array_create_2(_type, _n1length)   array_create_1dim(_n1length, sizeof(_type))
#define array_create_3(_type, _n1length, _n2length)   array_create_2dim(_n1length, _n2length, sizeof(_type*), sizeof(_type))
#define array_create_4(_type, _n1length, _n2length, _n3length)   array_create_3dim(_n1length, _n2length, _n3length, sizeof(_type**), sizeof(_type*), sizeof(_type))
#define array_create_5(_type, _n1length, _n2length, _n3length, _n4length)   array_create_4dim(_n1length, _n2length, _n3length, _n4length, sizeof(_type***), sizeof(_type**), sizeof(_type*), sizeof(_type))

// the actual implementation of array create
void *array_create_1dim(uint32_t _n1length, size_t t1);
void *array_create_2dim(uint32_t _n1length, uint32_t _n2length, size_t t1, size_t t2);
void *array_create_3dim(uint32_t _n1length, uint32_t _n2length, uint32_t _n3length, size_t t1, size_t t2, size_t t3);
void *array_create_4dim(uint32_t _n1length, uint32_t _n2length, uint32_t _n3length, uint32_t _n4length, size_t t1, size_t t2, size_t t3, size_t t4);

#endif




/** @brief vector_t is a vector container just like standart std vector in C++.

vector_t structure is a generic c-like array that can hold any type of variable in dynamically allocated memory.
Just like the arrays, vector data is contigous in memory and can be accessed externally.
The main advantage of the vector object onto the standart arrays is that the vector objects can dynamically grow or shrink.
This type of needs can be accomplished via realloc and memcpy opeartions in standart C and as expected IMLAB
is also doing these operations in clean and brief function calls and keeps everything about the vector in tiny and tidy vector_t structure.

\warning It is important to note that adding/deleting or changing an element or member of a vector should be done over the dedicated functions.
Setting an element of the vector over data pointer does not change the length nor the capacity of the vector. This could cause memory overflow or
data overwriting.

IMLAB defines the following functions for vector objects:
- #length         : returns the length of the vector
- #capacity       : returns the capacity of the vector
- #vdata / #data  : returns the pointer to the data of the vector
- #vector_create  : constructor function and zero initializer
- #vector_resize  : change the size of the data holded in container
- #vector_free    : destructor functions
- #vector_fill    : set the struct with a constant value
- #vector_permute : permute the entries of the vector with the given index list
- #vector_push    : push an instance to the vector, if there is no space, creates one
- #vector_pop     : get an index from the vector and remove it from the array
- #vector_read    : loads the vector struct from the hard drive (uncompress data)
- #vector_write   : saves the type struct to the hard drive (compress data)

Using a vector object is easy. Just decide the data type you need and call #vector_create to create a new vector object.
The data type can be any C data types, additional names declared in stdint.h or structs created by the user.
@code{.c}
vector_t out = vector_create(uint32_t);
@endcode
The above code will create a vector object and set the initial length to zero and capacity to one.
If you want to specify an initial capacity to vector just call:
@code{.c}
vector_t out = vector_create(float, 1024);
@endcode
This call of the #vector_create will create a floating point data pointer and allocate 1024*sizeof(float) byte memory on the memory.
Since the vector object could be enlarged or shrinked, setting an initial capacity will increase the time efficincy for large vectors.
It is also possible to create a vector from an existing one or from c pointers. In this case the third argument should be the pointer
to the data:
@code{.c}
double numbers[9] = {1,2,3, 4,5,6, 7,8,9};
vector_t out = vector_create(double, 9, numbers);
@endcode
In this case vector_create will allocate memory and do <b>memcpy</b> operation on the given pointer. To create a vector which
has initially constant values you can use #vector_fill function:
@code{.c}
vector_t out = vector_create(uint16_t, 100);
uint16_t val = 12;
vector_fill(&out, &val);
@endcode
In order to add/remove element to vector #vector_push/#vector_pop can be used.
@code{.c}
vector_t out = vector_create(float);
float values[12] = {0.0 0.1, 0.3, 0.5, 0.7, 0.9, 0.8, 0.6, 0.5, 0.4, 0.2, 0.0};
// in this loop the length of the vector will automatically increase up to 12.
for(size_t i = 0; i < 12; i++) {
    vector_push(&out, &values[i]);
}
@endcode
We can get the values from the vector with various methods. In the simplest case if we know the data type of the vector,
we can obtain the data pointer and access the values via pointer just like standart c arrays.
@code{.c}
float *vector_data = vdata(out, 0);
for(size_t i = 0; i < length(out); i++) {
    float val = vector_data[i];
}
@endcode
Or we can access the data elemnt using the special function #at as follows:
@code{.c}
for(size_t i = 0; i < length(out); i++) {
    float val = at(float, out, i);
}
@endcode

You can also use the vector container with the custom defined structures. Here is an example of how to use vector container with the custom structures.

@code{.c}
struct person {
 uint32_t id;
 uint32_t birthday;
};
vector_t *people = vector_create(struct person);
struct person var = {1, 1990};
vector_push(people, &var);
 ...
vector_free(&people);
@endcode
The above code will generate a person structure and create a vector container for that structure. And push an instance of the structure into the people vector.
And clear the vector after all the operations are done.

@warning If the custom type has any dynamically allocated memory pointer an additional care must be taken in order to avoid memory leaks.

If the structure contains any pointer member than #matrix_free will not free the memory allocated for the member and result in memory leakage.
Lets look at the problem  with a similar example;
@code{.c}
struct person {
 uint32_t id;
 char *name;
};
vector_t *people = vector_create(struct person, 50);
struct person var = {1, strdup("any dynamically allocated memory inside the custom type will cause memory leak")};
vector_push(people, &var);
 ...
vector_free(&people);
@endcode
In this case the size of the structure is 40 byte but freeing 40 byte will only remove the memory of id and name variable but not the memory allocated for the name pointer.
The only way to completely clean the object is first freeing the name pointer and than calling the #vector_free.

In order to make this process simple, IMLAB defines a function called #vector_destructor. This function is taking a destructor function for the created type and during #vector_free
IMLAB first calls the given destructor function and then free up the memory.

@code{.c}
struct person {
 uint32_t id;
 char *name;
};
// create a destructor for the person type
void person_destructor(void *elements, uint32_t plength) {
    struct *people = elements;
    for(size_t i=0; i < plength; i++) {
        free(people[i].name);
    }
}
vector_t *people = vector_create(struct person, 50);
// set destructor for the contained type
vector_destructor(people,  person_destructor);

struct person var = {1, strdup("in this case this memory will be released by the user defined destructor: person_destructor()")};
vector_push(people, &var);
 ...
vector_free(&people);
@endcode



@see vector_create
@see vector_resize
@see vector_free
@see vector_fill
@see vector_permute
@see vector_push
@see vector_pop
@see vector_read
@see vector_write

*/
typedef struct _vector { struct imlab_type_t *_type; uint32_t _capacity, _length; void *_data; }     vector_t;



/**
@hideinitializer
@brief Create a new vector with the specified input paramaters.
This macro definition calls the vector_create function with the appropriate number of arguments.
This macro overloaded with respect to number of arguments. The actual function takes three input arguments
so the macro implementation could take up to three arguments as follows:
\verbatim
vector_create(<type>, uint32_t capacity = 1, void *pointer = NULL)
@endverbatim
Missing arguments are replaced with the default values.
In the simplest case the function could take the only type of the vector.
This type could be any c types defined in stdint.h or struct created by the user.
@code{.c}
vector_t *out = vector_create(uint32_t);
@endcode
The above code will create a vector object and set the initial length to zero and capacity to one.
If you want to specify an initial capacity to vector just call:
@code{.c}
vector_t *out = vector_create(float, 1024);
@endcode
This call of the #vector_create will create a floating point data pointer and allocate 1024*sizeof(float) byte memory on the memory.
Since the vector object could be enlarged or shrinked, setting an initial capacity will increase the time efficincy for large vectors.
It is also possible to create a vector from c pointers. In this case the third argument should be the pointer
to the data:
@code{.c}
double numbers[9] = {1,2,3, 4,5,6, 7,8,9};
vector_t *out = vector_create(double, 9, numbers);
@endcode
In this case vector_create will allocate memory and do <b>memcpy</b> operation on the given pointer.

@param <type> Content type of the vector data. It could be any type char,uint32_t, float or user defined structures.
@param capacity Capacity of the output vector. If you dont know the exact size of the
vector at the programming stage just write the expected length of the vector. IMLAB automatically
increase the size if the data exceeds the vector length (This is only true if you use vector_push for the data insertion).
@param pointer A pointer to the vector data. If this parameter is send to the function imlab will allocate a new
memory and copies the send pointer into the container.
\return A vector_t object.
*/
#define vector_create(...) call(vector_create, __VA_ARGS__)

// not include these macros in the documentation
#ifndef DOXYGEN_SKIP_IMLAB_HIDDEN_MACRO_NAMES
// create an empty vector which has NULL pointer
#define vector_create_1(_type)                    vector_create_(type(_type), 1, NULL)
// create a vector with the specified size and allocate memory for it
#define vector_create_2(_type, _capacity)         vector_create_(type(_type), max_2(1, _capacity), NULL)
// create a vector with the specified size and use the given pointer as the container
#define vector_create_3(_type, _capacity, _ptr)   vector_create_(type(_type), max_2(1, _capacity), _ptr)
// call_with_default(3, va_nargs(__VA_ARGS__), vector_create_, T(arg_1(__VA_ARGS__)), rest(1,__VA_ARGS__), uint8_t,1,NULL)
// create a new vector with the given type and size information
vector_t *vector_create_(struct imlab_type_t *_type, uint32_t _capacity, void *ptr);

#endif

/**
 * Create a NULL vector. This vector will hold sizeof(vector_t) bytes of memory and should be used for all vector pointers that are not created
 * at the declaration.
 * @return
 */
vector_t* vector_null();

/**
Resize the input vector to the specified size.
@param var Input vector to be resized.
@param ncapacity New capacity of the input vector. If the new length is larger than the current length,
the contents of the vector is protected. If the new length is smaller than the current length, the #(new length) of the
input vector is protected.
*/
return_t vector_resize(vector_t *var, uint32_t _capacity);

/**
Pushes an element into the end of the vector. If the length of the vector exceeds the vector length
it automatically allocate memory for the new data.
@param _var Input vector.
@param _element Address of the data to be pushed.
*/
return_t vector_push(vector_t *var, void *element);

/**
Pops an element from the end of the vector and deletes the last element of the vector.
This method does not change the capacity of the vector. You can manually resize the vector
if you want to use less memory.
@param _var Input vector.
@param _element Address of an element to be popped into.
*/
return_t vector_pop(vector_t *var, void *element);

/**
Sets the idx'th element of the vector to the given value. This is not the correct way to insert an element to the vector.
Use vector_push for adding a new element to the vector.
This function is intented to be used internally or in loops so in order to be fast it doesnt control the idx and it will create a serious bug
if you try to insert an element at any index larger than the length of the vector and it will create a buffer overflow if index is larger than the capacity of the vector.
@param var Input vector.
@param idx Index of the vector element to be set.
@param value Address of the data to be set.
*/
void vector_set(vector_t *var, uint32_t idx, void *value);

/**
Gets the idx'th element of the vector to the given variable. This is not the correct way to extract an element from the vector.
Use vector_pop for extracting an element from the vector.
This function is intented to be used internally or in loops so in order to be fast it doesnt control the idx and it will create a serious bug
if you try to get an element at any index larger than the length of the vector and it will create a buffer overflow if index is larger than the capacity of the vector.
@param var Input vector.
@param idx Index of the vector element to be set.
@param value Address of the data to be set.
*/
void vector_get(vector_t *var, uint32_t idx, void *value);

/**
Permute the entries of the vector with the given index list.
@param _var Input vector to be permuted.
@param _idx Index list for the permuted vector.
*/
return_t vector_permute(vector_t *in, uint32_t *index_list);

/**
Release the allocated memory for the vector object.
@param _var Input vector to be freed.
*/
void vector_free(vector_t **var);

/**
Release the allocated memory by the vector object and reallocate with 1 length memory.
@param _var Input vector to be freed.
*/
void vector_reset(vector_t **var);

    /**
 * Sets a destructor for the data holded in the vector container
 * @param in Input vector
 * @param func Destructor function to be set
 */
    void vector_destructor(vector_t *in, void (*func)(void *, uint32_t));

/**
 *
 * @param var Input vector to be filled.
 * @param value Pointer to the variable to be filled into the vector element.
 * @return
 */
return_t vector_fill(vector_t *var, void *value);


/**
Save the input vector to the given file.
@param filename Output file name with an appropriate extension.
@param src Input vector structure to be saved.
The default option (0) saves the file without any compression.
*/
return_t vector_write(vector_t *src, const char *filename);

/**
Read the input vector from the given file.
@param filename Input file name with an appropriate extension.
@return Read vector from the file
*/
vector_t* vector_read(const char *filename);

/**
 * Return the unique elements of the vector
 * @param src Input vector
 * @param uniques Unique elements created with the same type of the source
 * @param unique_idx Index of the unique elements
 * @return
 */
return_t vector_unique(vector_t *src, vector_t *uniques, vector_t *unique_idx);

/**
 * Print the properties of the vector container
 * @param in Input vector to be viewed
 */
void vector_view(vector_t *in);


/**@hideinitializer
 * @brief This method converts the input vector (t1 typed) into t2 typed vector by just casting values.
 *
\code{.c}
float data[4] = {1.1, 2.2, 3.3, 4.4};
matrix_t *vecA = vector_create(float, 4, data);
// print the type name
printf("Type of vecA: %s\n", typename(vecA));
// print the values
printf("[%3.2f %3.2f %3.2f %3.2f]\n", at(float, vecA, 0), at(float, vecA, 1), at(float, vecA, 2), at(float, vecA, 3));
// convert the values into int type
vector_convert(float, int32_t, vecA);
// print the type name
printf("Type of vecA: %s\n", typename(matA));
// print the values
printf("[%d %d %d %d]\n", at(int32_t, vecA, 0), at(int32_t, vecA, 1), at(int32_t, vecA, 2), at(int32_t, vecA, 3));

// the above code will print
// Type of vecA: float
// [1.10 2.20 3.30 4.40]
// Type of vecA: int32_t
// [1 2 3 4]
\endcode

\warning Note that changing the access type of #at function does not change the type of the matrix or the data in the memory.
In #at or #data, type variable is used to determine the size of one element and jump to the correct index.
 */
#define vector_convert(_t1, _t2, _in) do { \
    _t2 *_imlab_temp_array = (_t2*)malloc(capacity(_in) * sizeof(_t2));\
    if(_imlab_temp_array != NULL) {\
        uint32_t _imlab_temp_integer = 0;\
        _t1 *_imlab_temp_in_data1 = data(_t1, _in);\
        for(_imlab_temp_integer=0; _imlab_temp_integer < length(_in); _imlab_temp_integer++) {\
            _imlab_temp_array[_imlab_temp_integer] = (_t2)_imlab_temp_in_data1[_imlab_temp_integer];\
        }\
    }\
    free((_in)->_data);\
    imlab_type_free(&(_in)->_type);\
    (_in)->_type = type(_t2);\
    (_in)->_data = _imlab_temp_array;\
}while(0);\


/**
 * Returns the data pointer of the vector
 * @param this Input vector
 * @return Pointer to the start of the vector data
 */
extern inline void* vdata(vector_t *this, uint32_t idx);
/**
 * Returns the length of the vector
 * @param this Input vector
 * @return Length of the input vector
 */
extern inline uint32_t length(vector_t *this);
/**
 * Returns the capacity of the vector
 * @param this Input vector
 * @return Capacity of the input vector
 */
extern inline uint32_t capacity(vector_t *this);

/**
 * Returns the type of the vector
 * @param this Input vector
 * @return Type information of the input vector
 */
extern inline struct imlab_type_t *vector_type(vector_t *this);



/** @brief matrix_t is a n-channel 2D matrix container.

matrix_t structure is a generic c-like array that can hold any type of variable in dynamically allocated memory.
Just like the arrays and vectors, matrix data is contigous in memory and can be accessed externally.
Similar to vector objects, matrix object is also resizable but you cannot increase the size of the matrix by pushing elements.
The main advantage of the matrix object onto the standart arrays is that the matrix objects can hold two dimensional data in a simple way.
Using matrix object the user interactions with the 2D array becomes easier and the code would be understandable.

IMLAB defines the following functions for vector objects:
- #width          : returns the number of coloumns of the matrix
- #cols           : returns the number of coloumns of the matrix
- #height         : returns the number of rows of the matrix
- #rows           : returns the number of rows of the matrix
- #channels       : returns the number of channels of the matrix
- #volume         : returns the volume (rows*cols*channels) of the matrix
- #mdata / #data  : returns the pointer to the data of the matrix
- #matrix_create  : constructor function zero initializer
- #matrix_resize  : change the size of the data holded in container
- #matrix_free    : destructor functions
- #matrix_fill    : set the struct with a constant value
- #matrix_read    : loads the matrix struct from the hard drive (uncompress data)
- #matrix_write   : saves the type struct to the hard drive (compress data)
- #matrix_copy    : creates a copy of the input matrix (size and data) and returns it

Using a vector object is easy. Just decide the data type you need and call #vector_create to create a new vector object.
The data type can be any C data types, additional names declared in stdint.h or structs created by the user.
@code{.c}
matrix_t *out = matrix_t(uint32_t);
@endcode
The above code will create a matrix object and set the initial rows, cols and channels to zero and data pointer to the NULL.
If you want to specify an initial size for the matrix just call:
@code{.c}
matrix_t out = matrix_create(float, 1024, 1024);
@endcode
This call of the #matrix_create will create a floating point data pointer and allocate 1024*1024*sizeof(float) byte memory on the memory.
If you want to marix to have more than one channels, you can add the third term as the number of channels.
It is also possible to create a matrix from an existing ones or from c pointers. In this case the fourth argument should be the pointer
to the data:
@code{.c}
double numbers[9] = {1,2,3, 4,5,6, 7,8,9};
matrix_t *out = matrix_create(double, 3,3,1, numbers);
@endcode
In this case matrix_create will allocate memory and do <b>memcpy</b> operation on the given pointer. If you want to create a matrix which
has initially constant values you can use #matrix_fill function:
@code{.c}
matrix_t *out = matrix_create(uint16_t, 100, 100, 2);
uint16_t val[2] = {12, 16};
matrix_fill(out, &val);
@endcode
The above code will generate a 100 x 100 matrix which has two dimensional inputs. The #matrix_fill function will set all the first channel elements to 12 and the second channel elements to 16.

We can get the values from the matrix with various methods. In the simplest case if we know the data type of the matrix,
we can obtain the data pointer and access the values via pointer just like standart c arrays.
@code{.c}
matrix_t *out;
float *matrix_data = mdata(out);
for(size_t i = 0; i < volume(out); i++) {
    float val = matrix_data[i];
}
@endcode
Or we can access the data element using the special function #at as follows:
@code{.c}
matrix_t *out;
for(size_t r = 0; i < rows(out); r++) {
    for(size_t c = 0; c < cols(out); c++) {
        float val = at(float, out, r,c);
    }
}
@endcode

You can also use the matrix container with the custom defined structures.  Here is an example of how to use matrix container with the custom structures.
@code{.c}
struct person {
 uint32_t id;
 uint32_t birthday;
};
matrix_t *people = matrix_create(struct person, 5, 5);
struct person var = {1, 1990};
// we can set a matrix element using at
at(struct person, people, 0,0) = var;
// or we can use matrix_set
matrix_set(people, 0,0,0, &var);
 ...
matrix_free(&people);
@endcode
The above code will generate a person structure and create a matrix container for that structure. Set the first element of the matrix and deallocate the memory when the all job is done.

@warning If the custom type has any dynamically allocated memory pointer an additional care must be taken in order to avoid memory leaks.

If the structure contains any pointer member than #matrix_free will not free the memory allocated for the member and result in have memory leakage.
Lets look at the problem  with a similar example;
@code{.c}
struct person {
 uint32_t id;
 char *name;
};
matrix_t *people = matrix_create(struct person, 5, 5);
struct person var = {1, strdup("any dynamically allocated memory inside the custom type will cause memory leak")};
// we can set a matrix element using at
at(struct person, people, 0,0) = var;
 ...
matrix_free(&people);
@endcode
In this case the size of the structure is 40 byte but freeing 40 byte will only remove the memory of id and name variable but not the memory allocated for the name pointer.
The only way to completely clean the object is first freeing the name pointer and than calling the #matrix_free.

In order to make this process simple, IMLAB defines a function called #matrix_destructor. This function is taking a destructor function for the created type and during #matrix_free
IMLAB first calls the given destructor function and then free up the memory.

@code{.c}
struct person {
 uint32_t id;
 char *name;
};
// create a destructor for the person type
void person_destructor(void *elements, uint32_t plength) {
    struct *people = elements;
    for(size_t i=0; i < plength; i++) {
        free(people[i].name);
    }
}
matrix_t *people = matrix_create(struct person, 5, 5);
// set the destructor for the matrix
matrix_destructor(people, person_destructor);

struct person var = {1, strdup("in this case this memory will be released by the user defined destructor: person_destructor()")};
at(struct person, people, 0,0) = var;
...
matrix_free(&people);
@endcode


@see matrix_create
@see matrix_resize
@see matrix_free
@see matrix_fill
@see matrix_load
@see matrix_save

*/
typedef struct _matrix { struct imlab_type_t *_type; uint32_t _rows, _cols, _channels; void *_data; } matrix_t;


/**
@hideinitializer
@brief Creates a new matrix element with the specified paramaters.
This macro overloaded with respect to number of arguments. The actual function takes five input arguments
so the macro implementation could take up to five arguments as follows:
\verbatim
matrix_create(<type>, uint32_t rows = 0, uint32_t cols = 0, uint32_t channels = 0, void *pointer = NULL)
@endverbatim
Missing arguments are replaced with the default values.
In the simplest case the function could take the only type of the matrix.
This type could be any c types defined in stdint.h or struct created by the user.
@code{.c}
matrix_t *out = matrix_create(uint32_t);
@endcode
The above code will create a matrix object and set the initial size to zero.
If you want to specify the size of the matrix at the construction call:
@code{.c}
matrix_t *out = matrix_create(float, 1024, 1024);
@endcode
This call of the #matrix_create will create a floating point data pointer and allocate 1024*1024*sizeof(float) byte memory on the memory.
It is also possible to create a matrix from an existing one or from c pointers. In this case the third argument should be the pointer
to the data:
@code{.c}
double numbers[9] = {1,2,3, 4,5,6, 7,8,9};
// create a new matrix from the given C array
matrix_t *out = matrix_create(double, 3,3,1, numbers);
// create a new matrix with the type and size of the out
matrix_t *out_copy = matrix_create(out, NULL);
// create a new matrix with the type, size and data of the out
matrix_t *out_clone = matrix_create(out, data(out));
@endcode
In this case matrix_create will allocate memory and do <b>memcpy</b> operation on the given pointer.

@param <type> Content type of the matrix data. It could be any type char,uint32_t, float or user defined structures.
@param rows Number of rows (height) of the output matrix.
@param cols Number of columns (width) of the output matrix.
@param channels Number of channels of the output matrix.
@param pointer A pointer to be set to the matrix data
*/
#define matrix_create(...) call(matrix_create, __VA_ARGS__)

// not include these macros in the documentation
#ifndef DOXYGEN_SKIP_IMLAB_HIDDEN_MACRO_NAMES

// helper functions for the matrix_create macro.
#define matrix_create_1(_type)                                matrix_create_(type(_type), 0, 0, 0, NULL, 0)
#define matrix_create_2(_var, _src_ptr)                       matrix_create_(imlab_type_copy(typeof(_var)), rows(_var), cols(_var), channels(_var), _src_ptr, 0)
// create a one channel matrix with the specified size
#define matrix_create_3(_type, _rows, _cols)                  matrix_create_(type(_type), _rows, _cols, 1, NULL, 0)
// create a matrix with the specified type and size row, col, channels
#define matrix_create_4(_type, _rows, _cols, _channels)       matrix_create_(type(_type), _rows, _cols, _channels, NULL, 0)
// create a matrix with the specified type, size and data pointer
#define matrix_create_5(_type, _rows, _cols, _channels, _ptr) matrix_create_(type(_type), _rows, _cols, _channels, _ptr, 0)
// create a matrix with the specified type, size and data pointer (do not allocate new memory use the given memory)
#define matrix_create_6(_type, _rows, _cols, _channels, _ptr, _use_pointer) matrix_create_(type(_type), _rows, _cols, _channels, _ptr, _use_pointer)
// actual function which does the job
matrix_t *matrix_create_(struct imlab_type_t *_type, uint32_t _rows, uint32_t _cols, uint32_t _channels, void *ptr, uint8_t use_pointer);

#endif

/**
 * Create a NULL matrix
 * @return
 */
matrix_t* matrix_null();

/**
 * @brief Set the value of a matrix element to the given variable
 * @param var Input matrix
 * @param rows Row of the matrix element to be set
 * @param cols Column of the matrix element to be set
 * @param channels Channel of the matrix element to be set
 * @param value Address of the variable to be set
 */
void matrix_set(matrix_t *var, uint32_t rows, uint32_t cols, uint32_t channels, void *value);

/**
 * @brief Set the given variable to the value of a matrix element in var(rows,cols,channels) position
 * @param var Input matrix
 * @param rows Row of the matrix element to be get
 * @param cols Column of the matrix element to be get
 * @param channels Channels channel of the matrix element to be get
 * @param value Address of the variable to be get into
 */
void matrix_get(matrix_t *var, uint32_t rows, uint32_t cols, uint32_t channels, void *value);

/**
Resize the input matrix to the specified size. This function is used most of the functions in order to create output matrix.
@param _var Input matrix to be resized.
@param _nrows New rows of the input matrix.
@param _ncols New columns of the input matrix.
@param _nchannels New channels of the input matrix.
*/
return_t matrix_resize(matrix_t *var, uint32_t nrows, uint32_t ncols, uint32_t nchannels);

/**
 * Convert the matrix into a vector
 * @param input Input matrix
 * @param order Row first or coloumn first conversion
 * @return Vector filled with matrix element
 */
vector_t* matrix2vector(matrix_t *input, uint8_t order);
/**
Release the allocated memory for the matrix object.
@param _var Input matrix to be freed.
*/
void matrix_free(matrix_t **var);

/**
 * Sets a destructor for the data holded in the matrix container
 * @param in Input matrix
 * @param func Destructor function to be set
 */
void matrix_destructor(matrix_t *in, void (*func)(void*, uint32_t));

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
return_t matrix_fill(matrix_t *out, void *value);

/**
 * Copy source matrix into the destination matrix, resize the destination if needed
 * @param src Source matrix to be copied
 * @param dst Destination matrix (same type with the source)
 * @return Success or relative error
 */
return_t matrix_copy(matrix_t *src, matrix_t *dst);

/**
 * Save the input matrix to the disk
 * @param src Input matrix to be saved
 * @param filename Filename for the output file
 * @return Success or relative error
 */
return_t matrix_write(matrix_t *src, const char *filename);

/**
 * Prints the matrix information into the stdout
 * @param in Input matrix to be viewed
 */
void matrix_view(matrix_t *in);

/**
 * Read the matrix from the file
 * @param filename Filename of the matrix
 * @param dst Non allocated matrix container for the destination
 * @return Success or relative error
 */
matrix_t* matrix_read(const char *filename);


/**@hideinitializer
 * @brief This method converts the input matrix (t1 typed) into t2 typed matrix by just casting values.
 *
\code{.c}
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

// the above code will print
// Type of matA: float
// [1.10 2.20 3.30 4.40]
// Type of matA: int32_t
// [1 2 3 4]
\endcode

\warning Note that changing the access type of #at function does not change the type of the matrix or the data in the memory.
In #at or #data, type variable is used to determine the size of one element and jump to the correct index.
 */
#define matrix_convert(_t1, _t2, _in) do { \
    _t2 *_imlab_temp_array = (_t2*)malloc(volume(_in) * sizeof(_t2));\
    if(_imlab_temp_array != NULL) {\
        uint32_t _imlab_temp_integer = 0;\
        _t1 *_imlab_temp_in_data1 = data(_t1, _in);\
        for(_imlab_temp_integer=0; _imlab_temp_integer < volume(_in); _imlab_temp_integer++) {\
            _imlab_temp_array[_imlab_temp_integer] = (_t2)_imlab_temp_in_data1[_imlab_temp_integer];\
        }\
    }\
    free((_in)->_data);\
    imlab_type_free(&(_in)->_type);\
    (_in)->_type = type(_t2);\
    (_in)->_data = _imlab_temp_array;\
}while(0);\


/**
 * Returns the type of the matrix
 * @param this Input matrix
 * @return Type information of the matrix
 */
extern inline struct imlab_type_t *matrix_type(matrix_t *this);

/**
 * Return the data pointer of the matrix
 * @param this Input matrix
 * @return Pointer to the data of the matrix
 */
extern inline void* mdata(matrix_t *this, uint32_t idx);

/**
 * Returns the width of the matrix
 * @param this Input matrix
 * @return Width of the input matrix
 */
extern inline uint32_t width(matrix_t *this);
/**
 * Returns the height of the matrix
 * @param this Input matrix
 * @return Height of the input matrix
 */
extern inline uint32_t height(matrix_t *this);
/**
 * Returns the rows of the matrix
 * @param this Input matrix
 * @return Rows of the input matrix
 */
extern inline uint32_t rows(matrix_t *this);
/**
 * Returns the columns of the matrix
 * @param this Input matrix
 * @return Columns of the input matrix
 */
extern inline uint32_t cols(matrix_t *this);
/**
 * Returns the channels of the matrix
 * @param this Input matrix
 * @return Channels of the input matrix
 */
extern inline uint32_t channels(matrix_t *this);
/**
 * Returns the volume of the matrix
 * @param this Input matrix
 * @return Volume (Rows * Cols * Channels) of the input matrix
 */
extern inline uint32_t volume(matrix_t *this);



/**
 * @brief string_t is a string type to hold char pointers as string
 */
typedef struct _string { uint32_t _length; uint32_t _capacity; char *_data; } string_t;

/**
 * Create a string from the given char pointer
 * @param cstr Input C string (char*)
 * @return string_t object
 */
string_t string(char *cstr);

/**
 * Return the C-compatible string of the string object
 * @param str A string_t object
 * @return C-compatible string
 */
char *c_str(string_t str);

/**
 * Append the given C string into the string_t variable
 * @param cstr Input C string
 * @param str Output string_t object
 * @return Success or relative error
 */
return_t string_append(char *cstr, string_t *str);

/**
 * Merges the given two string_t object and writes the output string into the first element
 * @param out_str First string element to be merged and also the output of the merge operation
 * @param in_str Second string object to be merged with the first one
 * @return Success or relative error
 */
return_t string_merge(string_t *out_str, string_t *in_str);

/**
 * Destructor of string_t *data array
 * @param in Input string_t *array
 * @param length Length of the pointer
 */
void string_destruct(void *in, uint32_t length);

/**
 * Appends the given format string into the @param str.
 * @param str Output string
 * @param format Format string to be written
 * @param ...
 * @return
\code{.c}
// first create an empty string
string_t stream = string("");
// fill the string with the given text
string_printf(&stream, "hello");
string_printf(&stream, "%c", 32);
string_printf(&stream, "IML@B");
// print the string
printf("%s\n", c_str(stream));

// the above code will print
// hello IML@B
\endcode
 */
return_t string_printf(string_t *str, const char *format, ...);

return_t string_restart(string_t *str);

/**
 * @brief Rectangle typed structure
 */
struct rectangle_t {
    int32_t x, y, width, height;
    float coefficient;
};
/**
 * Create a new rectangle with the given parameters
 * @param x x position of the top-left corner
 * @param y x position of the top-left corner
 * @param width Width of the rectangle
 * @param height Height of the rectangle
 * @param coefficient A floating point number assign to the rectangle (it could be detection score, color, opacity vs)
 * @return A rectangle object created in stack
 */
struct rectangle_t rectangle(int32_t x, int32_t y, int32_t width, int32_t height, float coefficient);

/**
 * Computes the overlapping area between the two rectangle objects.
 * @param box1 First rectangle object
 * @param box2 Second rectangle object
 * @param mode 0 for overlapping area between the two rectangle and 1 for normalized intersection area
 * @return Area of the intersection
 */
float rectangle_overlap(struct rectangle_t r1, struct rectangle_t r2, uint8_t mode);

/**
 * Merge given rectange vector and return the merged rectangles.
 * @param rect Vector of rectangles to be merged
 * @param overlap_threshold Minimum normalized intersection area. If any two rectangle has more overlap than this threshold, two vectors are assumed to be connected.
 * @param min_groups_size Minimum number of element in each group. If the number of connected components are less than this value, group will be discarded.
 */
vector_t *rectangle_merge(vector_t *rect, float overlap_threshold, uint32_t min_groups_size);

/**
 * @brief 3D point type
 */
struct point_t {
    float x, y, z;
};

/**
 * @brief Create a new point with the given parameters
 * @param x x position of the point
 * @param y y position of the point
 * @param z z position of the point
 * @return A point object created in stack
 */
struct point_t point(float x, float y, float z);

/**
 * @brief Computes the distance between the given two points
 * @param p1 First point
 * @param p2 Second point
 * @return Euclidean distance between the points
 */
float point_distance(struct point_t p1, struct point_t p2);

/**
 * @brief Fit minimum area rectangle to the input points by using the algorthm presented in "Finding best-ﬁtted rectangle for regions using a bisection method"
 * @param input Array of points
 * @param input_length Length of the input array
 * @return  Returns the four vertices (TopLeft, TopRight, BottomLeft, BottomRight) of the minimum area recctangle fitted to the input points.
 */
return_t point_fit_rectangle(struct point_t *input, uint32_t input_length, struct point_t output[4]);

#endif
