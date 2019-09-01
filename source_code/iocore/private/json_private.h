#ifndef IMLAB_IOCORE_JSON_PRIVATE_H
#define IMLAB_IOCORE_JSON_PRIVATE_H
// include the generic io module
#include "iocore_private.h"


/*
IMLAB JSON type names
All possible JSON data formats are listed in here as the integer holding macros.
We define json_type_t as macro instead of using enum so that we can use to store more data
insdie json_t. For example we can store the length of the data in the type value.
*/
#define JSON_TYPE_NULL      0 // this will be used if the data is not exist
#define JSON_TYPE_BOOLEAN   1 // these are the all possible json object types
#define JSON_TYPE_STRING    2 // these are the all possible json object types
#define JSON_TYPE_NUMBER    3 // these are the all possible json object types
#define JSON_TYPE_ARRAY     4 // these are the all possible json object types
#define JSON_TYPE_OBJECT    5 // these are the all possible json object types
#define JSON_TYPE_SERIAL    6 // this (may or may not) be used for serialization of the given object
/** @endcond */
// declare the JSON specific functions/macros/variables

#define JSON_MAX_KEY_LENGTH 1024

/*
DESTRUCTOR FUNCTIONS FOR THE JSON DATA TYPE
*/
/** free the memory holded by the json array */
void json_array_destruct(void *in, uint32_t length);
/** free the memory holded by the json object */
void json_object_destruct(void *in, uint32_t length);



// DECLARATIONS OF THE PARSER FUNCTIONS
/** The followings are the helper functions included in the current version of the JSON library.
These are private to the user. So the name space will be changed.
**/
///@{
return_t json_parse_boolean(char *data, size_t *pos, size_t length, struct json_data_t *out);
return_t json_parse_string(char *data, size_t *pos, size_t length, struct json_data_t *out);
return_t json_parse_number(char *data, size_t *pos, size_t length, struct json_data_t *out);
return_t json_parse_value(char *data, size_t *pos, size_t length, struct json_data_t *out);
return_t json_parse_array(char *data, size_t *pos, size_t length, struct json_data_t *out);
return_t json_parse_object(char *data, size_t *pos, size_t length, struct json_data_t *out);
///@}

// DECLARATIONS OF THE SERIALIZER FUNCTIONS
/** The followings are the helper functions included in the current version of the JSON library.
These are private to the user. So the name space will be changed.
**/
///@{
return_t json_serialize_value(struct json_data_t input, string_t *buffer);
return_t json_serialize_object(vector_t *input, string_t *buffer);
return_t json_serialize_array(vector_t *input, string_t *buffer);

return_t json_serialize_value_file(struct json_data_t input, int *depth, FILE **buffer);
return_t json_serialize_object_file(vector_t *input, int *depth, FILE **buffer);
return_t json_serialize_array_file(vector_t *input, int *depth, FILE **buffer);
///@}

#endif
