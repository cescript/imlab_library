#ifndef IMLAB_IMCORE_PRIVATE_H
#define IMLAB_IMCORE_PRIVATE_H

#include "../imcore.h"
#include "../../core/private/core_private.h"

#ifdef _IMLAB_PLATFORM_WINDOWS
    // include windows header for win32 display functions
    #include <windows.h>
#elif _IMLAB_PLATFORM_UNIX
    // include xlib ...
#endif

struct window_t {
    // window identifiers
    char *display_name;
    uint32_t id;
    // keep the information about the current display window
    uint32_t width, height, bpp;
    uint8_t *data; // pointer to the image

#ifdef _IMLAB_PLATFORM_WINDOWS
    HWND hwnd; // handle to main window
    HDC hdc; // handle device context
    BITMAPINFO* binfo; // bitmap info
    HGDIOBJ image; // image
    HINSTANCE hinstance; // module handle
#elif  _IMLAB_PLATFORM_UNIX
    // do xlib stuff here
#endif

};

/**
 * Resize the device context and handler of the given window object and update the window data wrt given size.
 * @param in Input window object to be resized
 * @param width New width of the window
 * @param height New height of the window
 * @param bpp New bpp of the window (This cannot be changed after create)
 * @param sign Use signed coordinates or use positive image coordinates
 * @return Positive if successful or negative if it fails.
 */
int window_resize(struct window_t *in, uint32_t width, uint32_t height, uint32_t bpp, uint32_t sign);




#endif
