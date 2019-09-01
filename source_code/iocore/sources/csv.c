#include <string.h>
#include <stdlib.h>
#include "private/iocore_private.h"

#define CSV_IS_COMMA(_x) ((_x) == 44)
#define CSV_IS_DOUBLEQUOTE(_x) ((_x) == 34)
#define CSV_IS_NEWLINE(_x) ((_x) == 10 || (_x) == 13)

struct csv_t {

    uint32_t cols;

    uint32_t current_row;
    unsigned char **current_row_data;

    uint32_t buffer_size;
    char *buffer;

    FILE *file;
};

// parse the single value, return the row increment
int find_an_element(FILE *stream)
{
    int c1, c2, clast = -1;

    // get the first character
    c1 = fgetc(stream);

    // non complex value, read it until the first comma or newline and return
    if(!CSV_IS_DOUBLEQUOTE(c1))
    {
        while(!feof(stream))
        {
            c1 = fgetc(stream);

            if(CSV_IS_COMMA(c1) || CSV_IS_NEWLINE(c1))
            {
                clast = c1;
                break;
            }
        }
    }
        // we are inside a complex value, comma, new line or double quotes might be the part of the value
    else
    {
        while(!feof(stream))
        {
            c1 = fgetc(stream);

            // if we encounter with quote, next char must be checked
            if(CSV_IS_DOUBLEQUOTE(c1))
            {
                c2 = fgetc(stream);
                if(CSV_IS_DOUBLEQUOTE(c2))
                {
                    // double quote
                }
                else
                {
                    // end of value
                    clast = c2;
                    break;
                }
            }
        }
    }
    return clast;
}

uint32_t find_number_of_columns(FILE *stream)
{
    uint32_t cols = 0;

    while(!feof(stream))
    {
        int clast = find_an_element(stream);

        if(clast == -1)
        {
            fprintf(stderr, "unexpected error!\n");
            return 0;
        }

        cols++;

        if(CSV_IS_NEWLINE(clast))
        {
            break;
        }
    }

    // set the cursor back to the starting position
    rewind(stream);

    return cols;
}

// parse the single value, return the row increment
int scan_single_col(FILE *stream, unsigned char **value, char *buffer, uint32_t max_buffer_length)
{
    uint32_t current_length = 0;
    int c1, c2, clast = -1;

    // get the first character, skip spaces
    while(!feof(stream) && (c1 = fgetc(stream)) == 32);

    // non complex value, read it until the first comma or newline and return
    if(!CSV_IS_DOUBLEQUOTE(c1))
    {
        while(!feof(stream) && current_length < max_buffer_length)
        {
            if(CSV_IS_COMMA(c1) || CSV_IS_NEWLINE(c1))
            {
                clast = c1;
                break;
            }

            buffer[current_length++] = (char)c1;
            c1 = fgetc(stream);
        }
    }
    // we are inside a complex value, comma, new line or double quotes might be the part of the value
    else
    {
        while(!feof(stream) && current_length < max_buffer_length)
        {
            c1 = fgetc(stream);

            // if we encounter with quote, next char must be checked
            if(CSV_IS_DOUBLEQUOTE(c1))
            {
                c2 = fgetc(stream);
                if(CSV_IS_DOUBLEQUOTE(c2))
                {
                    // double quote
                    buffer[current_length++] = c2;
                }
                else
                {
                    // end of value
                    clast = c2;
                    break;
                }
            }
            else
            {
                buffer[current_length++] = (char) c1;
            }
        }
    }

    if(current_length == max_buffer_length)
    {
        buffer[current_length-1] = '\0';
        printf("buffer: %s\n", buffer);
        fprintf(stderr, "row buffer size is not enough!\n");
        return -1;
    }

    // try to resize the current row data
    unsigned char *temp = realloc(value[0], current_length + 1);
    if(temp == NULL)
    {
        fprintf(stderr, "not enough memory for %d length data!\n", current_length);
        return -1;
    }
    value[0] = temp;
    memcpy(value[0], buffer, current_length);
    value[0][current_length] = '\0';

    return clast;
}

int scan_single_row(FILE *stream, unsigned char **out, uint32_t cols, char *buffer, uint32_t max_buffer_length)
{
    // try to find the length of the first line
    int clast = -1;

    uint32_t column = 0;
    while(!feof(stream))
    {
        clast = scan_single_col(stream, &out[column++], buffer, max_buffer_length);

        if(CSV_IS_NEWLINE(clast))
        {
            break;
        }
    }

    return column == cols;
}


struct csv_t* csv_open(char *filename, uint32_t skip_rows, uint32_t buffer_size)
{
    // try to open the file
    FILE *tmp = fopen(filename, "r");
    // return NULL if it fails
    check_file(tmp, NULL);

    // create the output csv file
    struct csv_t* out = (struct csv_t*) malloc(sizeof(struct csv_t));

    // check for the memory
    check_memory(out)
    {
        fclose(tmp);
        return NULL;
    }

    out->cols = 0;
    out->current_row = 0;
    out->current_row_data = NULL;
    out->buffer_size = 0;
    out->buffer = NULL;
    out->file = tmp;

    // get the number of columns
    out->cols = find_number_of_columns(out->file);

    // allocate columns
    out->current_row_data = (unsigned char**) malloc(out->cols * sizeof(unsigned char*));
    for(uint32_t i = 0; i < out->cols; i++)
    {
        out->current_row_data[i] = NULL;
    }

    // allocate buffer
    out->buffer_size = buffer_size;
    out->buffer = (char*) malloc(out->buffer_size);

    // skip the rows
    for(uint32_t i = 0; i < skip_rows; i++)
    {
        csv_get_next_line(out);
    }

    return out;
}

// parse the single line of csv data into the given variable length argument list
int csv_get_next_line(struct csv_t *in)
{
    check_null(in, 0);

    // increase the number of the lines
    if(scan_single_row(in->file, in->current_row_data, in->cols, in->buffer, in->buffer_size))
    {
        in->current_row++;
        return 1;
    }
    // not scanned
    return 0;
}

long csv_get_long(struct csv_t *in, uint32_t col)
{
    if(in == NULL)
    {
        return 0;
    }

    long value = 0;
    if(col < in->cols)
    {
        value = strtol(in->current_row_data[col], NULL, 0);
    }
    return value;
}

char* csv_get_string(struct csv_t *in, uint32_t col)
{
    if(in == NULL)
    {
        return 0;
    }

    char *value = NULL;
    if(col < in->cols)
    {
        value = in->current_row_data[col];
    }
    return value;
}

double csv_get_double(struct csv_t *in, uint32_t col)
{
    if(in == NULL)
    {
        return 0;
    }

    double value = 0;
    if(col < in->cols)
    {
        value = strtod(in->current_row_data[col], NULL);
    }
    return value;
}

// get number of columns in the csv file
uint32_t csv_get_column_size(struct csv_t* in)
{
    if(in == NULL)
    {
        return 0;
    }

    return in->cols;
}

// close the csv file and deallocate the buffers
void csv_close(struct csv_t **out)
{
    if(out[0] == NULL)
    {
        return;
    }

    // deallocate columns
    for(uint32_t i = 0; i < out[0]->cols; i++)
    {
        if(out[0]->current_row_data[i] != NULL)
        {
            free(out[0]->current_row_data[i]);
        }
    }
    if(out[0]->current_row_data != NULL)
    {
        free(out[0]->current_row_data);
    }
    // deallocate buffer
    if(out[0]->buffer != NULL)
    {
        free(out[0]->buffer);
    }

    // close the file handle
    if(out[0]->file != NULL)
    {
        fclose(out[0]->file);
    }

    out[0]->cols = 0;
    out[0]->current_row = 0;
    out[0]->buffer_size = 0;

    // free the single instance of csv object
    free(out[0]);
}

// undefine the macros
#undef CSV_IS_COMMA
#undef CSV_IS_DOUBLEQUOTE
#undef CSV_IS_NEWLINE