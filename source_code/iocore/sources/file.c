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

long int get_number_of_lines(FILE *in, long int start)
{
    // return 0 if the file is not valid
    if (in == NULL)
    {
        return 0;
    }

    // define necessary variables
    int cur = 0;
    long int new_lines = 0;

    // get the current position of the input file
    long int current = ftell(in);

    // set the current position to given starting position
    fseek(in, start, SEEK_SET);

    // read each character
    while ((cur = fgetc(in)) != EOF)
    {
        if (cur == '\n')
        {
            new_lines++;
        }
    }

    // set the input (current) position back to the input
    fseek(in, current, SEEK_SET);
    
    // return the counter numbers
    return new_lines;
}