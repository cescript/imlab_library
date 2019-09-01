//
// Created by cescript on 7.01.2018.
//
#include "private/core_private.h"
#include "private/iocore_private.h"
#include "private/json_private.h"

void tabulate(FILE *buffer, int *depth)
{
    if (buffer != NULL)
    {
        int p;
        for (p = 0; depth != NULL && p < depth[0]; p++)
        {
            fprintf(buffer, "\t");
        }
    }
}


return_t json_serialize_array(vector_t *input, string_t *buffer) {
    
    // write the opening bracket
    string_printf(buffer, "[");
   
    if (length(input) > 0)
    {
        int i;
        struct json_data_t *in = data(struct json_data_t, input);

        for (i = 0; i < length(input) - 1; i++)
        {
            json_serialize_value(in[i], buffer);
            // write comma for the next
            string_printf(buffer, ",");
        }
        json_serialize_value(in[i], buffer);
    }       
    // write the closing bracket
    string_printf(buffer, "]");
    
    // if anywhere in the process is ret_val is zero than we didnot manage to serialize
    return SUCCESS;
}

return_t json_serialize_array_file(vector_t *input, int *depth, FILE **buffer)
{
    return_t ret_val = SUCCESS;
    // write the opening bracket
    fprintf(buffer[0], "[");

    if (length(input) > 0)
    {
        int i;
        struct json_data_t *in = data(struct json_data_t, input);

        for (i = 0; i < length(input) - 1; i++)
        {
            ret_val = json_serialize_value_file(in[i], depth, buffer);
            // write comma for the next
            fprintf(buffer[0], ",");
        }
        ret_val = json_serialize_value_file(in[i], depth, buffer);
    }
    // write the closing bracket
    fprintf(buffer[0], "]");

    // if anywhere in the process is ret_val is zero than we didnot manage to serialize
    return ret_val;
}

return_t json_serialize_object(vector_t *input, string_t *buffer) {

    // write the opening bracket
    string_printf(buffer,  "%c",123);//{

    if (length(input) > 0)
    {
        int i;
        struct json_object_t *in = data(struct json_object_t, input);

        for (i = 0; i < length(input) - 1; i++)
        {
            // first write the name than write the value
            string_printf(buffer, "%c%s%c:", 34, in[i].name, 34); // add quotes before and after the object name
            json_serialize_value(in[i].element, buffer);
            // write comma for the next
            string_printf(buffer, ",");
        }
        string_printf(buffer, "%c%s%c:", 34, in[i].name, 34); // add quotes before and after the object name
        json_serialize_value(in[i].element, buffer);
    }
        
    // write the closing bracket
    string_printf(buffer, "%c", 125);//}

    // if anywhere in the process is ret_val is zero than we didnot manage to serialize
    return SUCCESS;
}

return_t json_serialize_object_file(vector_t *input, int *depth, FILE **buffer)
{
    // add tab for pretty output
    tabulate(buffer[0], depth);

    // write the opening bracket
    fprintf(buffer[0], "%c", 123); //{

    // increase the depth
    if (depth != NULL)
    {
        fprintf(buffer[0], "\n");
        depth[0]++;
    }

    if (length(input) > 0)
    {
        int i;
        struct json_object_t *in = data(struct json_object_t, input);
        for (i = 0; i < length(input) - 1; i++)
        {
            // add tab for pretty output
            tabulate(buffer[0], depth);

            // first write the name than write the value
            fprintf(buffer[0], "%c%s%c:", 34, in[i].name, 34); // add quotes before and after the object name
            json_serialize_value_file(in[i].element, depth, buffer);

            // write comma for the next
            fprintf(buffer[0], ",");

            if (depth != NULL)
            {
                fprintf(buffer[0], "\n");
            }
        }

        // add tab for pretty output
        tabulate(buffer[0], depth);

        fprintf(buffer[0], "%c%s%c:", 34, in[i].name, 34); // add quotes before and after the object name
        json_serialize_value_file(in[i].element, depth, buffer);
    }

    if (depth != NULL)
    {
        fprintf(buffer[0], "\n");
        depth[0]--;
    }

    // add tab for pretty output
    tabulate(buffer[0], depth);

    // write the closing bracket
    fprintf(buffer[0], "%c", 125);//}
    // if anywhere in the process is ret_val is zero than we didnot manage to serialize
    return SUCCESS;
}

return_t json_serialize_value(struct json_data_t input, string_t *buffer) {

    return_t ret_val = ERROR;
    // create a null data and return it if there is an error during parsing
    uint32_t value_type = input.type;

    if(value_type == JSON_TYPE_STRING) 
    {
        //printf("we found a string attribute\n");
        char *val = (char*)input.value;
        ret_val = string_printf(buffer, "\"%s\"", val);
    } 
    else if(value_type == JSON_TYPE_NUMBER) 
    {
        //printf("we found a number attribute\n");
        double val = ((double*)input.value)[0];
        ret_val = string_printf(buffer, "%f", val);
    } 
    else if(value_type == JSON_TYPE_BOOLEAN) 
    {
        //printf("we found a primitive attribute\n");
        uint8_t val = ((uint8_t*)input.value)[0];
        const char *arr[3] = {"false", "true", "null"};
        // use sprintf to write the data
        ret_val = string_printf(buffer, "%s", arr[val]);
    } 
    else if(value_type == JSON_TYPE_OBJECT) 
    {
        //printf("we found an object\n");
        ret_val = json_serialize_object((vector_t*)input.value, buffer);
    } 
    else if(value_type == JSON_TYPE_ARRAY) 
    {
        //printf("we found an array attribute\n");
        ret_val = json_serialize_array((vector_t*)input.value, buffer);
    }

    return ret_val;
}

return_t json_serialize_value_file(struct json_data_t input, int *depth, FILE **buffer)
{

    return_t ret_val = ERROR;
    // create a null data and return it if there is an error during parsing
    uint32_t value_type = input.type;

    if(value_type == JSON_TYPE_STRING) 
    {
        //printf("we found a string attribute\n");
        char *val = (char*)input.value;
        ret_val = fprintf(buffer[0], "\"%s\"", val);
    } 
    else if(value_type == JSON_TYPE_NUMBER) 
    {
        //printf("we found a number attribute\n");
        double val = ((double*)input.value)[0];
        ret_val = fprintf(buffer[0], "%f", val);
    } 
    else if(value_type == JSON_TYPE_BOOLEAN) 
    {
        //printf("we found a primitive attribute\n");
        uint8_t val = ((uint8_t*)input.value)[0];
        const char *arr[3] = {"false", "true", "null"};
        // use sprintf to write the data
        ret_val = fprintf(buffer[0], "%s", arr[val]);
    } 
    else if(value_type == JSON_TYPE_OBJECT) 
    {
        //printf("we found an object\n");
        if (depth)
        {
            fprintf(buffer[0], "\n");
        }
        ret_val = json_serialize_object_file((vector_t *)input.value, depth, buffer);

    } 
    else if(value_type == JSON_TYPE_ARRAY)
    {
        //printf("we found an array attribute\n");
        ret_val = json_serialize_array_file((vector_t *)input.value, depth, buffer);
    }
    // we cannot trace the error given by the fprintf
    return ret_val;
}

return_t json_serialize(vector_t *input, string_t *buffer) {

    return_t ret_val = WARNING_NOTHING_DONE;
    // serialize whatever in the given object to the buffer
    if(is_json_array(input)) {
        ret_val = json_serialize_array(input, buffer);
    } else if(is_json_object(input)) {
        ret_val = json_serialize_object(input, buffer);
    }
    return ret_val;
}

return_t json_write(const char *filename, vector_t *input, int mode) {

    int depth = 0;

    // open a file and serialize the JSON into it
    FILE *fp = fopen(filename, "w");
    check_file(fp, ERROR_UNABLE_TO_OPEN);

    return_t ret_val = WARNING_NOTHING_DONE;
    // serialize whatever in the given object to the buffer
    if(is_json_array(input)) {
        ret_val = json_serialize_array_file(input, mode > 0 ? &depth : NULL, &fp);
    } else if(is_json_object(input)) {
        ret_val = json_serialize_object_file(input, mode > 0 ? &depth : NULL, &fp);
    } else {
        vector_view(input);
    }
    fclose(fp);

    return ret_val;
}
