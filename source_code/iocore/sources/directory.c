//
// Created by cescript on 13.04.2018.
//
#include "private/iocore_private.h"

#ifdef _IMLAB_PLATFORM_WINDOWS
    #include <direct.h>
    #include <errno.h>
#elif _IMLAB_PLATFORM_UNIX
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

// create a directory with the given name
return_t imlab_mkdir(const char *pathname) {
    return_t status = SUCCESS;
#ifdef _IMLAB_PLATFORM_WINDOWS
    if(_mkdir(pathname) != 0 && errno == ENOENT) {
        status = ERROR_UNABLE_TO_OPEN;
    }
#elif _IMLAB_PLATFORM_UNIX
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


