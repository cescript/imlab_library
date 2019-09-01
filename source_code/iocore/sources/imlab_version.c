#include "private/iocore_private.h"

FILE *imlab_fopen(const char *filename, const char *mode, const char key[64]) {

    // try to open the file
    FILE *fp = fopen(filename, mode);
    // if the file is not opened, go back
    if(fp == NULL) {
        return NULL;
    }
    // check that the mode
    if(mode[0] == 'w' && key != NULL) {
        // write the given key value pair to the file
        fwrite(key, sizeof(char), 64, fp);
    }
    else if(mode[0] == 'r' && key != NULL) {
        // try to get the file version
        long int fsize = get_file_size(fp, 0);
        if(fsize < 64) {
            fclose(fp);
            message(ERROR_TYPE_MISMATCH, "given file is not an imlab file");
            fp = NULL;
        }
        // read the key
        char key_read[64];
        // read the given key value pair to the file
        fread(key_read, sizeof(char), 64, fp);
        // if the given key matches the target, return file
        if(strcmp(key, key_read)) {
            fclose(fp);
            message(ERROR_TYPE_MISMATCH, "given file type and content does not match");
            fp = NULL;
        }
    }
    return fp;
}