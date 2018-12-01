#ifndef __IMLAB_IOCORE_H__
#define __IMLAB_IOCORE_H__

#include "core.h"


/**
\description CSV is simple but very efficient CSV (Comma Seperated Values) reader library designed for IMLAB Image Processing Library. It has has been written in C89 standart and
in the same exact way with the IMLAB library. It is designed to be readable, easy to use and easy to remember. It has a single header (import and use) file and all the
functions are created under the json_ namespace.
A simple code written with CSV looks like this.

\code{.cpp}

struct csv_t *table = csv_open("sample.csv", 0, 1024);
int cols = csv_get_column_size(table);

// scan the header line
csv_get_next_line(table);

// print the header
for(int i = 0; i < cols; i++)
 {
    printf("%s \t ", csv_get_string(table, i));
 }
printf("\n");
// print the values
while(csv_get_next_line(table))
 {
    int id = csv_get_long(table, 0);
    char *name = csv_get_string(table, 1);
    double score = csv_get_double(table, 2);

    printf("[%02d] \t [%s] \t [%3.2f]\n", id, name, score);
 }

csv_close(&table);
\endcode

*/

/**
 * Open a csv file and return the handler
 * @param filename Input filename with csv extension
 * @param skip_rows Number of rows to be skipped
 * @param buffer_size Max length of the single cell element in bytes (typical: 1024)
 * @return A pointer to the created csv structure
 */
struct csv_t* csv_open(char *filename, uint32_t skip_rows, uint32_t buffer_size);

/**
 * Returns the number of columns in the current csv file
 * @param in Input pointer to the csv structure
 * @return Number of columns in the file
 */
uint32_t csv_get_column_size(struct csv_t* in);

/**
 * Scan the next row of the current file and parse the cells
 * @param in Input pointer to the csv structure
 * @return Return the number scanned line (zero or one)
 */
int csv_get_next_line(struct csv_t *in);

// get the values of the current row
/**
 * Get the integer value in the current row and given column
 * @param in Input pointer to the csv structure
 * @param col Column index of the integer data (0 based)
 * @return Integer value of the cell (current row, col)
 */
long csv_get_long(struct csv_t *in, uint32_t col);

/**
 * Get the string value in the current row and given column
 * @param in Input pointer to the csv structure
 * @param col Column index of the string data (0 based)
 * @return String value of the cell (current row, col)
 */
char* csv_get_string(struct csv_t *in, uint32_t col);

/**
 * Get the double value in the current row and given column
 * @param in Input pointer to the csv structure
 * @param col Column index of the double data (0 based)
 * @return Double value of the cell (current row, col)
 */
double csv_get_double(struct csv_t *in, uint32_t col);

/**
 * Close the given file and deallocates the memory
 * @param out Address of the pointer to the csv structure
 */
void csv_close(struct csv_t **out);

/**
 * Create a directory with the given name and set the mode to the given mode
 * @param pathname Name of the path
 * @return
 */
return_t imlab_mkdir(const char *pathname);

/**
 * Create and return a unique filename that is not in the current directory.
 * @param filename Name of the file
 * @param extension Extension of the filename
 * @return filename_%d.extension formatted char pointer
 */
char *imlab_filename(const char *filename, const char *extension);


/**
\file iocore.h
Here the defintions of the Input/output functions provided by the IMLAB library.
*/


/* DECLARIONS FROM THE JSON PARSER */

/**
\description JSON is simple but very efficient JSON reader library designed for IMLAB Image Processing Library. It has has been written in C89 standart and
in the same exact way with the IMLAB library. It is designed to be readable, easy to use and easy to remember. It has a single header (import and use) file and all the
functions are created under the json_ namespace.
A simple code written with JSON looks like this.

\code{.cpp}

vector_t *root = json_read("haarcascade_frontalface_alt.json");
int s = length(root);

int s1 = json_get_number(root, "size1", 0);
int s1 = json_get_number(root, "size2", 0);
printf("Cascade Box Size: %d x %d\n", s1, s2);

// we can get stages array from the current node using json_get_array
vector_t *stages = json_get_array(root, "stages", 0);

// stages is an array which holds a number and an array inside
for(i=0; i < length(stages); i++) {
    // while getting item from array just use two variable array name and index
    vector_t *obj = json_get_object(stages, i);
    double stage_threshold = json_get_number(obj, "thres", 0);
    vector_t *trees = json_get_array(obj, "trees", 0);
    for(j=0; j < length(trees); j++) {
        ...
    }
}
\endcode

As seen in the sample code, any JSON object or array is holded by the vector_t structure. In order to hold different types
in a single vector, IMLAB defines a new structure namely struct json_data_t. This structure is consist of a type identifier and
a void pointer to the real data. A JSON data could be created with the following constructors:

 \code{.cpp}

vector_t *root = vector_create(struct json_data_t, 10);

struct json_data_t temp;

// sets the temp to JSON boolean 0 (false)
json_boolean(0, &temp);
// inserts it into the root array
vector_push(root, &temp);

// sets the temp to 1.27
json_number(1.27, &temp);
// inserts it into the root array
vector_push(root, &temp);

// sets the temp to JSON string
json_string("This is a sample JSON string!", &temp);
// inserts it into the root array
vector_push(root, &temp);

// sets the temp to JSON object
vector_t *object = vector_create(struct json_object_t, 1);
json_object(object, &temp);
// inserts it into the root array
vector_push(root, &temp);

// sets the temp to JSON object
vector_t *array = vector_create(struct json_data_t, 1);
json_array(array, &temp);
// inserts it into the root array
vector_push(root, &temp);
\endcode


*/

/** An array element can hold number of different typed objects under its element pointer. A JSON array can be constructed by vector_create(struct json_data_t).
struct json_data_t is the key type for the all JSON library. It has a type indicator and a generic value holder.
This propoerty of json_data_t provides the JSON array to hold any data type.

JSON array is consist of pointer to the generic json data holder type json_t*. Each data can be accessed via its position in the array. IMLAB JSON library
provides the following helpful macros to get or set any value inside the array.
- json_push(vector_t*, data)
- json_get(vector_t*, id)
- json_get_boolean(vector_t*, id)
- json_get_string(vector_t*, id)
- json_get_number(vector_t*, id)
- json_get_array(vector_t*, id)
- json_get_object(vector_t*, id)
*/


/** An object element can hold a number of different type object and their names under its element pointer. A JSON object can be constructed by json_object_create().
- json_object_create(int capacity)  : This function creates an json_object_t object which can store #capacity json_t* inside

JSON object is consist of pointer to the generic json data holder type json_t*. Each data can be accessed by its name or id. IMLAB JSON library
provides the following helpful macros to get or set any value inside the object.
- json_push(json_t*, data, name)
- json_get(json_t*, name, id)
- json_get_id(object, name)
- json_get_name(object, id)
- json_get_boolean(json_t*, name, id)
- json_get_string(json_t*, name, id)
- json_get_number(json_t*, name, id)
- json_get_array(json_t*, name, id)
- json_get_object(json_t*, name, id)

If the object name is empty than the function will use the given id as the index and return the value.
This could be faster if the object has too many data elements and you already know the index of the element.
*/

/**
This data structure will hold a single data element which should have a type name and value.
JSON_T is the most generic container for any json type object. It holds a single data element
which should have a type name and value. Value is defined as void pointer so that it can hold all types of data without any problem.
In the user side this type is the generic input type for all the json functions and return types for most the json functions (excepts for the type specified functions).

The following functions accepts the json_t* input:
- json_get_length(json_t*) : This function return the length of the given data. If the data is static (number, string, primitive) it returns 1 otherwise the length of the given data.
- json_get_[boolean, number, string, array, object](json_t*, name, id) : Returns the data holded in the elements of the given data (object) with the specified type.
- json_get_[boolean, number, string, array, object](json_t*, id) : Returns the data holded in the elements of the given data (array) with the specified type.
*/

/*
CONSTRUCTOR FUNCTIONS FOR THE JSON DATA TYPE
*/
/**
This is the reader function for any json file. This function reads the json data in the given file and parse the all name value pairs into the json_t* data.
*/
vector_t* json_read(const char *filename);

/**
This is the serializer function for any json file. This function reads the parsed json data  and serialize it into the given file.
*/
return_t json_write(const char *filename, vector_t *input);

/**
 * Serialize the given JSON structure into the string buffer
 * @param input JSON structure created by json_create or json_read.
 * @param buffer Empty string pointer which will be filled by the function.
 * @param buffer_length Initial buffer length which will be updated by the function.
 * @return Return positive if no error occurs.
 */
return_t json_serialize(vector_t *input, string_t *buffer);

/**
 * Free up the memory holded by the json object
 * @param in Input json object to be freed
 * @return SUCCESS if the cleaning is succesfull
 */
return_t json_free(vector_t **in);


/** This is a constructor for JSON null object. This is usefull in order to prevent errors. */
vector_t *json_create(void);

/** \name IMLAB JSON get mehods
These are the definitions of the public functions for getting each element from the object or array.
Use these functions if you need any scalar or array type element from any json array or object. These functions are overloaded
based on the number of input arguments. So use json_get_X(data, name, id) to get a data from the object or json_get_X(data, id) from the array.
**/
//@{
/** @hideinitializer \brief This function will return the json_t* holded in object->element["name"]. */
#define json_get(...)         call(json_get, __VA_ARGS__)
/** @hideinitializer \brief This function will return the boolean (int8_t) holded in the object->element["name"]. */
#define json_get_boolean(...) call(json_get_boolean, __VA_ARGS__)
/** @hideinitializer \brief This function will return the string (char*) holded in the object->element["name"]. */
#define json_get_number(...)  call(json_get_number, __VA_ARGS__)
/** @hideinitializer \brief This function will return the number (double) holded in the object->element["name"]. */
#define json_get_string(...)  call(json_get_string, __VA_ARGS__)
/** @hideinitializer \brief This function will return the array (struct json_array_t*) holded in the object->element["name"]. */
#define json_get_array(...)   call(json_get_array, __VA_ARGS__)
/** @hideinitializer \brief This function will return the object (struct json_object_t*) holded in the object->element["name"]. */
#define json_get_object(...)  call(json_get_object, __VA_ARGS__)
//@}

#ifndef DOXYGEN_SKIP_IMLAB_HIDDEN_MACRO_NAMES
uint8_t   json_get_boolean_3(vector_t *input, char *name, int id);
double    json_get_number_3(vector_t *input, char *name, int id);
char*     json_get_string_3(vector_t *input, char *name, int id);
vector_t* json_get_array_3(vector_t *input, char *name, int id);
vector_t* json_get_object_3(vector_t *input, char *name, int id);

uint8_t   json_get_boolean_2(vector_t *input, int id);
double    json_get_number_2(vector_t *input, int id);
char*     json_get_string_2(vector_t *input, int id);
vector_t* json_get_array_2(vector_t *input, int id);
vector_t* json_get_object_2(vector_t *input, int id);
#endif



/* END OF JSON PARSER DECLARATIONS */

#endif