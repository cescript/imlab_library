#include <string.h>
#include <stdlib.h>
#include "private/iocore_private.h"
#include "private/json_private.h"


int json_push_3(vector_t *input, struct json_data_t value, char *name) {
    // create a new object
    struct json_object_t obj;
    json_object_create(name, value, &obj);
    // push it
    return vector_push(input, &obj);
}
/// insert a data element into the end of the given object
int json_push_2(vector_t *input, struct json_data_t item) {
    return vector_push(input, &item);
}


return_t json_object_create(char *name, struct json_data_t data, struct  json_object_t *out) {
    out->name = imlab_strdup(name);
    out->element = data;
    return SUCCESS;
};



// @checked
int json_get_id(vector_t* input, const char *element_name) {
    // check that the given vector is json_object
    check_condition(is_json_object(input), -1, "cannot search an element in a non object json element!");
    // get the
    struct json_object_t *root = data(struct json_object_t, input);
    int i = 0;
    for(i=0; i < length(input); i++) {
        if(strcmp(root[i].name, element_name) == 0) {
            return i;
        }
    }
    return -1;
}


// create getter functions which take 3 input arguments
int json_get_3(vector_t *input, char *name, int element_id, struct json_data_t *output) {

    if(strlen(name) > 0 || element_id > length(input) || element_id < 0) {
        element_id = json_get_id(input, name);
    }
    if(element_id == -1) {
        return -1;
    }
    // find the object in the vector
    struct json_object_t item = at(struct json_object_t, input, element_id);
    *output = item.element;
    // retru the value holded in that position
    return 1;
}
// create getter functions which take 2 input arguments
int json_get_2(vector_t *input, int element_id, struct json_data_t *output) {
    if(element_id < 0 || element_id >= length(input)) {
        return -1;
    }
    *output = at(struct json_data_t, input, element_id);
    return 1;
}


// create getter functions which take 2 input arguments
#define json_value_getter3(return_type, function_name, return_if_not_found) \
return_type cat_3(function_name,_,3)(vector_t *input, char *name, int id) {    \
    struct json_data_t item;                                                            \
    if(json_get_3(input, name, id, &item) == -1)  {                                     \
        message(ERROR, "cannot get the element");                                          \
        return return_if_not_found;                                                     \
    }                                                                                   \
    return ((return_type*)item.value)[0];                                              \
}

#define json_pointer_getter3(return_type, function_name, return_if_not_found)\
return_type cat_3(function_name,_,3)(vector_t *input, char *name, int id) {    \
    struct json_data_t item;                                                            \
    if(json_get_3(input, name, id, &item) == -1)  {                                     \
        message(ERROR, "cannot get the element");                                          \
        return return_if_not_found;                                                     \
    }                                                                                   \
    return ((return_type)item.value);                                                   \
}
/** These are the definitions of the get methods for each class. Note that these are defined to get values from the object.
Dont use them on arrays.
**/

json_value_getter3(uint8_t, json_get_boolean, 0);
json_value_getter3(double, json_get_number, 0.0);
// if the return type is pointer we return the value pointer directly (ptr)
json_pointer_getter3(char*, json_get_string, NULL);
json_pointer_getter3(vector_t*, json_get_array, NULL);
json_pointer_getter3(vector_t*, json_get_object, NULL);

// create getter functions which take 2 input arguments
#define json_value_getter2(return_type, function_name, return_if_not_found) \
return_type cat_3(function_name,_,2)(vector_t *input, int id) {                         \
    struct json_data_t item;                                                            \
    if(json_get_2(input, id, &item) == -1)  {                                           \
        message(ERROR, "cannot get the element");                                          \
        return return_if_not_found;                                                     \
    }                                                                                   \
    return ((return_type*)item.value)[0];                                               \
}

#define json_pointer_getter2(return_type, function_name, return_if_not_found) \
return_type cat_3(function_name,_,2)(vector_t *input, int id) {                            \
    struct json_data_t item;                                                            \
    if(json_get_2(input, id, &item) == -1)  {                                           \
        message(ERROR, "cannot get the element");                                          \
        return return_if_not_found;                                                     \
    }                                                                                   \
    return ((return_type)item.value);                                                   \
}

/** These are the definitions of the get methods for each class. Note that these are defined to get values from the arrays only.
Dont use them on objects or anything. These are private functions. Only call the public versions of it.
**/
json_value_getter2(uint8_t, json_get_boolean, 0);
json_value_getter2(double, json_get_number, 0.0);
// if the return type is pointer we return the value pointer directly (ptr)
json_pointer_getter2(char*, json_get_string,NULL);
json_pointer_getter2(vector_t*, json_get_array, NULL);
json_pointer_getter2(vector_t*, json_get_object, NULL);



// clean the memory
// implementation under this file

void json_object_destruct(void *par_jsdata, uint32_t length) {
    // we know that the json object is consist of json_object_t data
    struct json_object_t *js_data = par_jsdata;
    // now go over each data and free them sperately
    uint32_t i;
    for(i=0; i < length; i++) {
        // each object has a name and a json_data_t
        free(js_data[i].name);
        // get the type of the data and try to free it
        uint32_t t = js_data[i].element.type;

        if(t == JSON_TYPE_NULL) {
            // it is already freed
        } else if(t == JSON_TYPE_BOOLEAN || t == JSON_TYPE_NUMBER || t == JSON_TYPE_STRING) {
            // in this case the value shows the address, so we can free them directly
            free(js_data[i].element.value);
        } else if(t == JSON_TYPE_ARRAY) {
            //? ((struct json_object_t*)(in[0]->_data))[i].element.value;
            // in this case value shows a vector of json_data_t type
            vector_free((vector_t**)&(js_data[i].element.value));
        } else if(t == JSON_TYPE_OBJECT) {
            // in this case value shows a vector of json_data_t type
            vector_free((vector_t**)&(js_data[i].element.value));
            // @TODO consider that should we also clear the vector?
        }
    }
}

void json_array_destruct(void *par_jsdata, uint32_t length) {
    // we know that the json array is consist of json_data_t data
    struct json_data_t *js_data = par_jsdata;
    // now go over each data and free them seperately
    uint32_t i;
    for(i=0; i < length; i++) {
        // each data has a type and value pointer
        // get the type of the data and try to free it
        uint32_t t = js_data[i].type;

        if(t == JSON_TYPE_NULL) {
            // it is already freed
        } else if(t == JSON_TYPE_BOOLEAN || t == JSON_TYPE_NUMBER || t == JSON_TYPE_STRING) {
            // in this case the value shows the address, so we can free them directly
            free(js_data[i].value);
        } else if(t == JSON_TYPE_ARRAY) {
            // in this case value shows a vector of json_data_t type
            vector_free((vector_t**)&(js_data[i].value));
        } else if(t == JSON_TYPE_OBJECT) {
            // in this case value shows a vector of json_data_t type
            vector_free((vector_t**)&(js_data[i].value));
        }
    }
}
