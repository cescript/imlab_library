#include <string.h>
#include <stdlib.h>
#include "private/core_private.h"
#include "private/iocore_private.h"
#include "private/json_private.h"

// STATIC HIDDEN (ONLY DEFINED IN THIS FILE)
static char read_next_char(char *data, size_t *pos);
return_t json_read_object_name(char *data, size_t *pos, size_t length, char *name);

// EXTERN API
/*
// parse a boolean value from the given file data and insert the value into the given output
return_t json_parse_boolean(unsigned char *data, size_t *pos, size_t length, struct json_data_t *out);
return_t json_parse_string(unsigned char *data, uint32_t *pos, size_t length, struct json_data_t *out);
return_t json_parse_number(unsigned char *data, uint32_t *pos, size_t length, struct json_data_t *out);
return_t json_parse_value(unsigned char *data, int *pos, size_t length, struct json_data_t *out);
return_t json_parse_array(unsigned char *data, int *pos, int length, vector_t *out);
return_t json_parse_object(unsigned char *data, int *pos, int length, vector_t *out);
vector_t* json_read(const char *filename);
*/
// read the next printable character and return it as it is
static char read_next_char(char *data, size_t *pos)
{
    do {
        ++(*pos);
    } while(data[*pos] == 10 || data[*pos] == 11 || data[*pos] == 13 || data[*pos] == 32);
    return data[(*pos)];
};


/** This function will be called when we encounter with an " in the json file (as value)
and it will read the text and set the position to the next char after the ending ".
It does not consider any escape sequence so it is not appropriate for parsing strings.
**/
return_t json_read_object_name(char *data, size_t *pos, size_t length, char *name)
{

    uint32_t idx = 0;
    check_condition(data[(*pos)++] == '"', ERROR_TYPE_MISMATCH, "json object name must start with a quote");
    // find the end of the string

    while(*pos < length && data[*pos] != '"') {
        if(idx < JSON_MAX_KEY_LENGTH) {
            name[idx++] = data[*pos];
        }
        else if(idx == JSON_MAX_KEY_LENGTH){
            message(WARNING, "object name exceeds the maximum available object name length");
        }
        (*pos)++;
    }
    name[min_2(idx, JSON_MAX_KEY_LENGTH-1)] = '\0';
    check_condition(data[*pos] == '"', ERROR_TYPE_MISMATCH, "json object name must end with a quote");
    //
    return SUCCESS;
}

/* THE FOLLOWING CODE DEFINES THE PARSER FUNCTIONS FOR EACH TYPES */

/** This function will be called when we encounter with an T,F,N in the json file (as value)
and it will read the text and set the position to the next char after the ending of the T(E), F(E), N(L)
**/
return_t json_parse_boolean(char *data, size_t *pos, size_t length, struct json_data_t *out) {
    uint8_t value = 0;
    if(strncmp(data+*pos, "true", 4) == 0) {
        value = 1;
        *pos += 3;
    } else if(strncmp(data+*pos, "false", 5) == 0) {
        value = 0;
        *pos += 4;
    } else if(strncmp(data+*pos, "null", 4) == 0) {
        value = 2;
        *pos += 3;
    } else {
        message(ERROR, "A json boolean must be true, false or null!\n");
        return ERROR_TYPE_MISMATCH;
    }
    // try to insert the value into the out and return the result
    return json_boolean(value, out);
}


/** This function will be called when we encounter with an " in the json file (as value)
and it will read the text and set the position to the next char after the ending ".
Note that in the value we can encounter with different escape sequences so we need to consider them.
**/
return_t json_parse_string(char *data, size_t *pos, size_t length, struct json_data_t *out) {

    uint32_t start = *pos;
    uint32_t end   = (*pos)+1;

    check_condition(data[start] == '"', ERROR_TYPE_MISMATCH, "A json string must start with a quote");

    // find the end of the string
    while(end < length && data[end] != '"') {
        // if it is an escape sequence, dont read that as ""
        // we dont want to find some keywords in a string, so if a string is started
        if(data[end] == '\\' && data[end+1] == '"') {
            ++end;
        }
        ++end;
    }

    check_condition(data[end] == '"', ERROR_TYPE_MISMATCH, "A json string must end with a quote");

    uint32_t string_length = end - start - 1;
    // insert the data into the out
    out->type = JSON_TYPE_STRING;
    out->value = (unsigned char*) calloc(string_length+1, sizeof(char));
    check_memory(out->value, ERROR_OUT_OF_MEMORY);
    // do copy op
    memcpy(out->value, &data[start+1], string_length * sizeof(char));
    ((char*)out->value)[string_length] = '\0';
    //printf("str_len: %d\n", string_length);
    *pos = end;
    // all done
    return SUCCESS;
}


/** This function will be called when we encounter with a -, or 0...9 in the json file (as value)
and it will read the text and set the position to the next char after the ending of the number.
If the value is too high it will return an error and set the output variable to NULL
**/
return_t json_parse_number(char *data, size_t *pos, size_t length, struct json_data_t *out) {

    // read the first char
    unsigned char nc = data[*pos];

    check_condition(nc == '-' || (nc >= '0' && nc <= '9'), ERROR_TYPE_MISMATCH, "A json number must start with - or 0-9");

    char *end;
    double value = strtod(&data[*pos], &end);
    // if the given string is not number than the output pointer will be NULL
    // if the number is too big, errno will be set the ERANGE

    check_condition(end != NULL, ERROR_TYPE_MISMATCH, "Json number is too big or not a number");

    // end of the number is returned by the strtod
    *pos = end - (char*)data - 1;
    // try to insert the value into the out and return result
    return json_number(value, out);
}


/** This function is called when we want to parse a json value. It will simply create jumps based
on the starting value of the current character. For example if the current character is '{' then it will create
a json data which holds an json object and return it.
**/
return_t json_parse_value(char *data, size_t *pos, size_t length, struct json_data_t *out) {

    return_t ret_val = json_null(out);
    char nc = data[*pos];

    if(nc == '"') {
        //printf("we found a string attribute\n");
        ret_val = json_parse_string(data, pos, length, out);
    } else if(nc == '-' || (nc >= '0' && nc <= '9')) {
        //printf("we found a number attribute\n");
        ret_val = json_parse_number(data, pos, length, out);
    } else if(nc == 't' || nc == 'f' || nc == 'n') {
        //printf("we found a primitive attribute\n");
        ret_val = json_parse_boolean(data, pos, length, out);
    } else if(nc == 123) {
        //printf("we found an object\n");
        ret_val = json_parse_object(data, pos, length, out);
    } else if(nc == '[') {
        //printf("we found an array attribute\n");
        ret_val = json_parse_array(data, pos, length, out);
    }

    return ret_val;
}


// json parse object
return_t json_parse_array(char *data, size_t *pos, size_t length, struct json_data_t *out)
{

    // we will start from 0
    if(data[*pos] != '[') {
        printf("JSON Array must start with an %c but %c found at position %ld\n", 123, data[*pos], *pos);
        return ERROR_TYPE_MISMATCH;
    }
    // a temp data that will be pushed into the vector
    struct json_data_t *item = (struct json_data_t *) malloc(sizeof(struct json_data_t));
    vector_t *out_items = vector_create(struct json_data_t, 1);
    // get the starting character of the given string
    unsigned char nc = read_next_char(data, pos);

    while(nc != ']') {
        // try to parse the data and get the next json_item
        return_t is_parsed = json_parse_value(data, pos, length, item);
        if(is_parsed < 0) {
            vector_free(&out_items);
            return is_parsed;
        }
        // try to push the json_data into the array
        return_t is_pushed = vector_push(out_items, item);
        if(is_pushed < 0) {
            vector_free(&out_items);
            return is_pushed;
        }
        // we are end of the one item, this should continue with comma or closing bracket
        nc = read_next_char(data, pos);
        if(nc == ',') {
            nc = read_next_char(data, pos);
        }
    }
    free(item);
    return json_array(out_items, out);
}

// json parse object
return_t json_parse_object(char *data, size_t *pos, size_t data_length, struct json_data_t *out)
{

    // we will start from 0
    if(data[*pos] != '{') {
        printf("JSON Object must start with an %c but %c found at position %ld\n", 123, data[*pos], *pos);
        return ERROR_TYPE_MISMATCH;
    }
    // create a dummy object
    struct json_data_t *object = (struct json_data_t*) malloc(sizeof(struct json_data_t));
    vector_t *out_objects = vector_create(struct json_object_t, 1);
    // this will hold the name of the each object for
    char *object_name_holder = (char*) malloc(JSON_MAX_KEY_LENGTH * sizeof(char));
    // reads the next non whitespace character and sets the position
    char nc = read_next_char(data, pos);
    // read until the next char is closing brackets
    while(nc != 125) {

        // now if we are here that means the name of the object is exist
        // we will read the object name first
        return_t is_read = json_read_object_name(data, pos, data_length, object_name_holder);

        if(is_read < 0) {
            vector_free(&out_objects);
            free(object_name_holder);
            return is_read;
        }
        // name value pairs must be separeted by :
        nc = read_next_char(data, pos);
        if(nc != ':') {
            vector_free(&out_objects);
            free(object_name_holder);
            fprintf(stderr, "object name has found but the value is missing");
            return ERROR_TYPE_MISMATCH;
        }

        // if we are here, we are ready to read the value of the object/attribute
        nc = read_next_char(data, pos);
        // so there could be so many things, so we should examine each by turn

        // try to parse the value into that object
        return_t is_parsed = json_parse_value(data, pos, data_length, object);
        if(is_parsed < 0) {
            vector_free(&out_objects);
            free(object_name_holder);
            return is_parsed;
        }
        // try to push the json_data into the object
        return_t is_pushed = json_push(out_objects, object[0], object_name_holder);
        if(is_pushed < 0) {
            vector_free(&out_objects);
            free(object_name_holder);
            return is_pushed;
        }
        // after parsing everything this should end the while loop if the nc is not comma
        nc = read_next_char(data, pos);
        if(nc == ',') {
            nc = read_next_char(data, pos);
        }
        // all is done
    }
    // we dont need object_name_holder anymore
    free(object_name_holder);
    free(object);

    return json_object(out_objects, out);
}

vector_t* json_read(const char *filename) {

    struct json_data_t root;

    // open the file
    FILE *js = fopen(filename,"r");

    // check the file
    check_file(js, vector_null());

    // measure the length of the file
    fseek(js, 0, SEEK_END);
    size_t length = ftell(js);
    fseek(js, 0, SEEK_SET);
    // allocate sapce for the string buffer
    char *buffer = (char*) malloc (sizeof(char)*length);
    // copy the file into the buffer:
    size_t lengthRead = fread (buffer, 1, length, js);
    fclose(js);

    // check that reading done without any problem
    check_condition(lengthRead == length, vector_null(), "cannot read from file");

    // set the parsed data
    size_t pos = 0;

    // check BOM header 0xEF, 0xBB, 0xBF
    if (length > 3 && buffer[0] == (char)0xEF && buffer[1] == (char)0xBB && buffer[2] == (char)0xBF)
    {
        // skip the header and start from 3
        pos = 3;
    }

    // @TODO fix the return type and return the situation of the json_parse object function
    return_t ret_val = json_parse_object(buffer, &pos, length, &root);
    free(buffer);
    // set the output to the allocated vector
    check_return(ret_val, vector_null());

    // return output vector
    return (vector_t*)root.value;
}
