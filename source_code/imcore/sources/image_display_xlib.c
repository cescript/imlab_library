#include "private/imcore_private.h"

#ifdef UNIX

struct window_t* window_create(char *filename, uint32_t options) {
    message(WARNING_NOT_SUPPORTED, "Unix platform currently is not supported");
    return NULL;
}

// this function clears the previously allocated memory for the image and resize the window
int window_resize(struct window_t *in, uint32_t width, uint32_t height, uint32_t bpp, uint32_t sign)  {
    message(WARNING_NOT_SUPPORTED, "Unix platform currently is not supported");
    return -1;
}

// this loop waits sleep ms and handles the messages
int window_wait(struct window_t *display, int sleep) {
    message(WARNING_NOT_SUPPORTED, "Unix platform currently is not supported");
    return -1;
}

void imshow(matrix_t *image, struct window_t *display) {
    message(WARNING_NOT_SUPPORTED, "Unix platform currently is not supported");
    return;
}

#endif