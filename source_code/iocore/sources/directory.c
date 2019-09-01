//
// Created by cescript on 13.04.2018.
//
#include <unistd.h>
#include "private/iocore_private.h"

#ifdef WIN32
    #include <direct.h>
    #include <errno.h>
#elif UNIX
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

// create a directory with the given name
return_t imlab_mkdir(const char *pathname) {
    return_t status = SUCCESS;
#ifdef WIN32
    if(_mkdir(pathname) != 0 && errno == ENOENT) {
        status = ERROR_UNABLE_TO_OPEN;
    }
#elif UNIX
    status = mkdir(pathname, 777)  == 0 ? SUCCESS:ERROR_UNABLE_TO_OPEN;
#else
    message(ERROR_UNABLE_TO_OPEN, NULL);
#endif
    // check status and return it
    check_return(status, status);
    return status;
}

char *imlab_split_filename_and_extension(const char *filename_with_extension, char *filename, char *extension) {

    int32_t i = strlen(filename_with_extension);
    while(--i > 0 && filename_with_extension[i] != '.')
    {
        extension[i] = filename_with_extension[i];
    }
    while(--i)
    {
        filename[i] = filename_with_extension[i];
    }

}


char *imlab_filename(const char *filename, const char *extension) {

    uint32_t i = 1;
    // create an imlab string which automatically grows
    string_t output_filename = string("");
    // search the all files until the boundary of the 2^32
    while(i > 0) {
        string_printf(&output_filename, "%s_%d.%s", filename, i++, extension);
        // check that the file is exist
        if(access( output_filename._data, F_OK ) == -1) {
           return output_filename._data;
        }
        string_restart(&output_filename);
    }
    // return the filename
    return output_filename._data;
}


