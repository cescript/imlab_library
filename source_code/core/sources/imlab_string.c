//
// Created by cescript on 6.01.2018.
//
#include <string.h>
#include <stdlib.h> // for memcpy
#include "private/core_private.h"



/* first implement some string operations for the C string (char*)) */

// manual implementation of strdup function
char *imlab_strdup(const  char *cstr) 
{
    size_t slen = strlen(cstr) + 1;
    char *p = (char*)malloc(slen * sizeof(char));
    
    if(p != NULL) 
    {
        memcpy(p, cstr, slen-1);
        p[slen-1] = '\0';
    }
    return p;
}

// create a string
string_t string(const char *cstr) 
{
    string_t str;
    // fill the string
    str._length = strlen(cstr);
    str._capacity = str._length;
    str._data   = imlab_strdup(cstr);
    
    // return the filled string
    return str;
}

// return C string of the string
char *c_str(string_t str) 
{
    return str._data;
}

return_t string_printf(string_t *str, const char *format, ...) 
{
    // get the starting position
    va_list args;
    va_start(args, format);
    int necessary = vsnprintf(NULL, 0, format, args) + 1;

    if(necessary > (int)(str->_capacity - str->_length)) 
    {
        uint32_t allocSize = max_2(str->_capacity + necessary, 2*str->_capacity);
        char *temp = (char*) realloc(str->_data, allocSize * sizeof(char));
        if(temp != NULL) 
        {
            str->_data     = temp;
            str->_capacity = allocSize;
        } 
        else 
        {
            return ERROR_OUT_OF_MEMORY;
        }
    }
    /* now we have memory write it */
    str->_length += vsnprintf(str->_data + str->_length, str->_capacity - str->_length, format, args);
    va_end(args);

    return SUCCESS;
}

return_t string_restart(string_t *str) 
{
    // set the start of the string to the beginning of the string
    if(str->_capacity > 0) 
    {
        str->_data[0] = '\0';
        str->_length = 0;
    }
    else 
    {
        return ERROR;
    }
    return SUCCESS;
}

// append a cstring to end of the strig
return_t string_append(const char *cstr, string_t *str) 
{
    // update the existing string
    uint32_t clen = strlen(cstr);
    char *tmp = realloc(str->_data, (str->_length + clen) * sizeof(char));
    check_memory(tmp, ERROR_OUT_OF_MEMORY);

    // append the cstr into the string
    str->_data = tmp;
    memcpy(str->_data + str->_length, cstr, clen * sizeof(char));

    return SUCCESS;
}
// merges to string into the first
return_t string_merge(string_t *out_str, string_t *in_str) 
{
    uint32_t len = in_str->_length;
    char *tmp = realloc(out_str->_data, (out_str->_length + len) * sizeof(char));
    check_memory(tmp, ERROR_OUT_OF_MEMORY);

    // append the cstr into the string
    out_str->_data = tmp;
    memcpy(out_str->_data + out_str->_length, in_str->_data, len * sizeof(char));
    return SUCCESS;
}

// clean the data holded by the str
void string_free(string_t *str) 
{
    free(str->_data);
}

void string_destruct(void *par_in, uint32_t length) 
{
    string_t *in = par_in;
    uint32_t i;
    for(i=0; i < length; i++) 
    {
        string_free(in + i);
    }
}
