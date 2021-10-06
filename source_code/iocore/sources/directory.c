//
// Created by cescript on 13.04.2018.
//
#include "private/iocore_private.h"
#include <errno.h>
#ifdef _IMLAB_PLATFORM_WINDOWS
    #include <direct.h>
#elif _IMLAB_PLATFORM_UNIX
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

// create a directory with the given name
return_t imlab_mkdir(const char *pathname) {
    return_t status = ERROR_UNABLE_TO_OPEN;
#ifdef _IMLAB_PLATFORM_WINDOWS
    if(_mkdir(pathname) == 0 || errno == EEXIST) {
        status = SUCCESS;
    }
#elif _IMLAB_PLATFORM_UNIX
    if(mkdir(pathname, 777) == 0 || errno == EEXIST) {
        status = SUCCESS;
    }
#else
    message(status, NULL);
#endif
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


