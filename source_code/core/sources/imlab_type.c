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

/**
 * @file imlab_types.h
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

#ifndef __IMLAB_TYPES__
#define __IMLAB_TYPES__

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include "private/core_private.h"
#include "private/json_private.h"

// this function checks the return value of the previous function and prints the error message
int print_message_func(return_t cond, int line, const char *func, const char *fmt, ...) {

    // get the current time
    time_t rawTime = time(NULL);
    struct tm *localTime = localtime(&rawTime);

    // get the time as string
    char timeString[64];
    size_t strLength = strftime(timeString, sizeof(timeString), "%H:%M:%S", localTime);
    timeString[strLength] = '\0';

    if(is_error(cond) && IM_VERBOSE_ERROR)
    {
        fprintf(stderr, "\033[31m");
        fprintf(stderr, "%s error at %s on line %d: " , timeString, func, line);

        // print dialog
        if(fmt != NULL)
        {
            // create variable length argument list
            va_list args;

            va_start(args, fmt);
            vfprintf(stderr, fmt, args);
            va_end(args);
        }
        else
        {
            if (cond == ERROR_UNABLE_TO_READ)
            {
                fprintf(stderr, "unable to read data from the given source");
            }
            else if (cond == ERROR_NOT_IMAGE)
            {
                fprintf(stderr, "input must be an image");
            }
            else if (cond == ERROR_NULL_TYPE)
            {
                fprintf(stderr, "cannot process null type");
            }
            else if (cond == ERROR_TYPE_MISMATCH)
            {
                fprintf(stderr, "type mismatch");
            }
            else if (cond == ERROR_DIMENSION_MISMATCH)
            {
                fprintf(stderr, "dimension mismatch");
            }
            else if (cond == ERROR_OUT_OF_MEMORY)
            {
                fprintf(stderr, "not enough memory");
            }
            else if (cond == ERROR_UNABLE_TO_OPEN)
            {
                fprintf(stderr, "cannot open the file");
            }
            else
            {
                fprintf(stderr, "unknown");
            }
        }
        // close coloring
        fprintf(stderr, "\033[0m\n");
        fflush(stderr);

    }
    else if(is_warning(cond) && IM_VERBOSE_WARNING)
    {
        fprintf(stderr, "\033[33m");
        fprintf(stderr, "%s warning at %s on line %d: " , timeString, func, line);

        // print dialog
        if(fmt != NULL)
        {
            // create variable length argument list
            va_list args;

            va_start(args, fmt);
            vfprintf(stderr, fmt, args);
            va_end(args);
        }
        else
        {
            if(cond == WARNING_NOT_SUPPORTED) {
                fprintf(stderr, "not supported option or platform");
            }
            else if(cond == WARNING_NOTHING_DONE) {
                fprintf(stderr, "already done");
            }
            else {
                fprintf(stderr, "unknown");
            }
        }

        // close coloring
        fprintf(stderr, "\033[0m\n");
        fflush(stderr);
    }
    else if(is_success(cond) && IM_VERBOSE_SUCCESS)
    {
        fprintf(stderr, "\033[32m");
        fprintf(stderr, "%s success at %s on line %d: " , timeString, func, line);

        // print dialog
        if(fmt != NULL)
        {
            // create variable length argument list
            va_list args;

            va_start(args, fmt);
            vfprintf(stderr, fmt, args);
            va_end(args);
        }
        else
        {
            fprintf(stderr, "operation completed");
        }

        // close coloring
        fprintf(stderr, "\033[0m\n");
        fflush(stderr);
    }

    // check the return value and return true if it is an error
    return is_success(cond) ? 0:1;
}

// return the id of the given type
uint8_t imlab_type_id(struct imlab_type_t *tinfo) {
    if(tinfo == NULL) {
        return IMLAB_TYPE_NULL;
    }
    return tinfo->id;
}

uint16_t imlab_type_elemsize(struct imlab_type_t *tinfo) {
    if(tinfo == NULL) {
        return 0;
    }
    return tinfo->elemsize;
}

char* imlab_type_name(struct imlab_type_t *tinfo) {
    if(tinfo == NULL) {
        return "null";
    }
    return tinfo->name;
}



// clean the memory holded by the imlab type
void imlab_type_free(struct imlab_type_t **var) {
    if(var[0] != NULL) {
        return;
    }
    free(var[0]->name);
    free(var[0]);
}

struct imlab_type_t *imlab_type_copy(struct imlab_type_t *src) {
    if(src == NULL) {
        return NULL;
    }
    // allocate a space for the new data type
    struct imlab_type_t *this = malloc(sizeof(struct imlab_type_t));
    // check that the given type is not created before
    this->id         = src->id;
    this->elemsize   = src->elemsize;
    this->name       = imlab_strdup(src->name);
    this->destructor = src->destructor;
    // return the created value for the given type
    return this;
}


// this function will take an imlab type and create a 32 bit integer which includes important data
static uint8_t imlab_type_get_id(char *type_name) {

    uint8_t id = 0;
    if(!strcmp(type_name, "int8_t")) {
        id = IMLAB_TYPE_8S;
    } else if(!strcmp(type_name, "uint8_t")) {
        id = IMLAB_TYPE_8U;
    } else if(!strcmp(type_name, "int16_t")) {
        id = IMLAB_TYPE_16S;
    } else if(!strcmp(type_name, "uint16_t")) {
        id = IMLAB_TYPE_16U;
    } else if(!strcmp(type_name, "int32_t")) {
        id = IMLAB_TYPE_32S;
    } else if(!strcmp(type_name, "uint32_t")) {
        id = IMLAB_TYPE_32U;
    } else if(!strcmp(type_name, "float")) {
        id = IMLAB_TYPE_32F;
    } else if(!strcmp(type_name, "double")) {
        id = IMLAB_TYPE_64F;
    } else if(!strcmp(type_name, "point_t")) {
        id = IMLAB_TYPE_POINT;
    } else if(!strcmp(type_name, "rectangle_t")) {
        id = IMLAB_TYPE_RECTANGLE;
    } else if(!strcmp(type_name, "string_t")) {
        id = IMLAB_TYPE_STRING;
    } else if(!strcmp(type_name, "struct json_data_t")) {
        id = IMLAB_TYPE_JSON_ARRAY;
    } else if(!strcmp(type_name, "struct json_object_t")) {
        id = IMLAB_TYPE_JSON_OBJECT;
    } else if(!strcmp(type_name, "null")) {
        id = IMLAB_TYPE_NULL;
    } else {
        // we dont know the data type
        // but it is valid for imlab operations
        id = IMLAB_TYPE_VALID;
    }
    return id;
}


// create a new imlab type from the given type
// an imlab type consist of a string type name, integer type value (contains id and size of the type) and integer type id
struct imlab_type_t *imlab_type_create(char *type_name, uint32_t type_size) {
    // allocate a space for the new data type
    struct imlab_type_t *this = malloc(sizeof(struct imlab_type_t));
    // check that the given type is not created before
    uint32_t ID = imlab_type_get_id(type_name);
    this->id         = ID;
    this->elemsize   = type_size;
    this->name       = imlab_strdup(type_name);
    if(ID == IMLAB_TYPE_STRING) {
        this->destructor = string_destruct;
    } else if(ID == IMLAB_TYPE_JSON_ARRAY) {
        this->destructor = json_array_destruct;
    } else if(ID == IMLAB_TYPE_JSON_OBJECT) {
        this->destructor = json_object_destruct;
    } else {
        this->destructor = NULL;
    }

    // return the created value for the given type
    return this;
}

void imlab_type_set_destructor(struct imlab_type_t *_type, void (*func)(void*, uint32_t)) {
    _type->destructor = func;
}

return_t imlab_type_write(struct imlab_type_t* in, FILE *out) {
    // write the id information
    fwrite(&in->id, sizeof(uint8_t), 1, out);
    // write the element size
    fwrite(&in->elemsize,  sizeof(uint16_t), 1, out);
    // write the type name
    uint32_t name_length = strlen(in->name);
    fwrite(&name_length, sizeof(uint32_t), 1, out);
    fwrite(in->name, sizeof(char), name_length, out);
    // return the read type
    return SUCCESS;
}

struct imlab_type_t* imlab_type_read(FILE *in) {
    // create a new type
    struct imlab_type_t *out = (struct imlab_type_t*) malloc(sizeof(struct imlab_type_t));
    // read the id information
    fread(&out->id, sizeof(uint8_t), 1, in);
    // read the element size
    fread(&out->elemsize,  sizeof(uint16_t), 1, in);
    // read the type name
    uint32_t name_length = 0;
    fread(&name_length, sizeof(uint32_t), 1, in);
    out->name = calloc(name_length , sizeof(char));
    fread(out->name, sizeof(char), name_length, in);
    // return the read type
    return out;
}



#endif
