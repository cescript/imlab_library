//
// Created by cescript on 23.02.2018.
//
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char *getopt_find(char *option, char *param) {
    uint32_t optionLen = strlen(option);
    uint32_t paramLen  = strlen(param);
    uint32_t idx = 0;
    for(uint32_t i=0; i < optionLen; i++) {
        uint32_t ii = i, j = 0;
        while( (j < paramLen) & (ii < optionLen) && (option[ii++] == param[j++]));
        if(j == paramLen && option[ii] == ':') {
            idx = ii;
            break;
        }
    }
    char *start = NULL;
    if(idx != 0 && idx != optionLen) {
        start = option + idx;
    }
    return start;
}

uint32_t getopt_int32_t(char *option, char *param, int32_t *values) {
    uint32_t count = 0;
    // get the starting position of the parameter
    char *start, *end = getopt_find(option, param);
    // if the substing found, fill the given array
    if(end != NULL) {
        do {
            start = end + 1;
            values[count++] = (int32_t)strtol(start, &end, 10);
        } while(end[0] == 'x');
    }
    return count;
}

uint32_t getopt_uint32_t(char *option, char *param, uint32_t *values) {
    uint32_t count = 0;
    // get the starting position of the parameter
    char *start, *end = getopt_find(option, param);
    // if the substing found, fill the given array
    if(end != NULL) {
        do {
            start = end + 1;
            values[count++] = (uint32_t)strtoul(start, &end, 10);
        } while(end[0] == 'x');
    }
    return count;
}

uint32_t getopt_double(char *option, char *param, double *values) {
    uint32_t count = 0;
    // get the starting position of the parameter
    char *start, *end = getopt_find(option, param);
    // if the substing found, fill the given array
    if(end != NULL) {
        do {
            start = end + 1;
            values[count++] = strtod(start, &end);
        } while(end[0] == 'x');
    }
    return count;
}

uint32_t getopt_float(char *option, char *param, float *values) {
    uint32_t count = 0;
    // get the starting position of the parameter
    char *start, *end = getopt_find(option, param);
    // if the substing found, fill the given array
    if(end != NULL) {
        do {
            start = end + 1;
            values[count++] = (float)strtod(start, &end);
        } while(end[0] == 'x');
    }
    return count;
}
