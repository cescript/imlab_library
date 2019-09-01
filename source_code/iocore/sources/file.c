//
// Created by cescript on 14.04.2018.
//
#include <stdio.h>

long int get_file_size(FILE *in, long int start) {
    if(in == NULL) {
        return 0;
    }
    long int current = ftell(in);
    // set the cursor to the end
    fseek(in, 0, SEEK_END);
    long int end = ftell(in);
    // set the cursor to the starting position and start parsing
    fseek(in, current, SEEK_SET);
    // return the length of the file
    return end - start;
}
