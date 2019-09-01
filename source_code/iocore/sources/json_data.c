#include <string.h>
#include <stdlib.h>
#include "private/core_private.h"
#include "private/json_private.h"

// implementation under this file
/*
return_t json_null(struct json_data_t *out);
return_t json_boolean(int8_t value, struct json_data_t *out);
return_t json_string(unsigned char *value, struct json_data_t *out);
return_t json_number(double value, struct json_data_t *out);
return_t json_object(vector_t *value, struct json_data_t *out);
return_t json_array(vector_t *value, struct json_data_t *out);
return_t json_serial(struct json_data_t *out);
*/
/// insert a data element into the end of the given object
return_t json_null(struct json_data_t *out) {
    out->type = JSON_TYPE_NULL;
    out->value = NULL;
    // done we can return the element
    return SUCCESS;
}

return_t json_boolean(uint8_t value, struct json_data_t *out) {
    out->type = JSON_TYPE_BOOLEAN;
    out->value = malloc(sizeof(uint8_t));
    check_memory(out->value, ERROR_OUT_OF_MEMORY);
    memcpy(out->value, &value, sizeof(uint8_t));
    // done we can return the element
    return SUCCESS;
}

return_t json_string(char *value, struct json_data_t *out) {
    out->type = JSON_TYPE_STRING;
    out->value = calloc(strlen(value)+1, sizeof(char));
    check_memory(out->value, ERROR_OUT_OF_MEMORY);
    memcpy(out->value, value, strlen(value)*sizeof(char));
    // done we can return the element
    return SUCCESS;
}

return_t json_number(double value, struct json_data_t *out) {
    out->type = JSON_TYPE_NUMBER;
    out->value = malloc(sizeof(double));
    check_memory(out->value, ERROR_OUT_OF_MEMORY);
    memcpy(out->value, &value, sizeof(double));
    // done we can return the element
    return SUCCESS;
}

// value is a json_object_t typed vector
return_t json_object(vector_t *value, struct json_data_t *out) {
    out->type = JSON_TYPE_OBJECT;
    // point the json_data to the real object data holder
    out->value = value;
    // return everything you have
    return SUCCESS;
}
// value is a json_data_t typed vector
return_t json_array(vector_t *value, struct json_data_t *out) {
    out->type = JSON_TYPE_ARRAY;
    // point the json_data to the real array data holder
    out->value = value;
    // return everything you have
    return SUCCESS;
}
