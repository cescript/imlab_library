#include "private/imcore_private.h"

#ifdef _IMLAB_PLATFORM_WINDOWS


LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    // get the window parameters from the window handle
    struct window_t *display = (struct window_t*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    // check that the data field of the display structure is allocated
    if(display == NULL || display->data == NULL) {
        return DefWindowProc(hwnd, msg, wparam, lparam);
    } else if(msg == WM_CLOSE) {
        PostQuitMessage(0);
    } else if(msg == WM_PAINT) {
        // draw the latest device context into the window
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(display->hwnd, &ps);
        BitBlt(hdc, 0, 0, display->width, display->height, display->hdc, 0, 0, SRCCOPY );
        EndPaint(display->hwnd, &ps);
    }
    // otherwise return the default window response
    return DefWindowProc(hwnd, msg, wparam, lparam);
}


int init(HINSTANCE hinstance) {
    // create a window class ex and initalize it
    WNDCLASSEX ex;
    // init ex
    ex.cbSize = sizeof(WNDCLASSEX); // size of the registered class
    ex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // how to redraw window The Main Window's Style
    ex.lpfnWndProc = WinProc; // Message Processing (takes the messages from the GUI and this function process it)
    ex.cbClsExtra = 0; // additional Memory Request None
    ex.cbWndExtra = 0; // Window Extra-Memory
    ex.hInstance = hinstance;
    ex.hIcon = LoadIcon(NULL, IDI_APPLICATION); // The Application Main Icon
    ex.hCursor = LoadCursor(NULL, IDC_ARROW); // cursor type
    ex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH); // The Window's Background Color
    ex.lpszMenuName = NULL; // if we want to draw menu inside the main window, tell the name
    ex.lpszClassName = "IMLAB_WINDOW"; // name of the generated class
    ex.hIconSm = NULL;

    RegisterClassEx(&ex);

    return 0;
}



struct window_t* window_create(char *filename, uint32_t options) {
    // create a new window object
    struct window_t *out = (struct window_t*) malloc(sizeof(struct window_t));
    // set the display name
    out->display_name = imlab_strdup(filename);
    out->hinstance = GetModuleHandle(NULL);
    check_null(out->hinstance, NULL, "cannot get the instance to the executable");
    // set the initial values to zero
    out->image = 0; // image will be allocated by SelectObject (in window resize)
    out->binfo = NULL; // info will be allocated in window resize
    out->data = NULL; // data will be allocated by the Win32 CreateDIBSection function
    out->hwnd = NULL; // window will be allocated by create window function
    out->hdc = NULL; // device context will be read from the window handler
    out->id = 0; // reserved
    // register the window
    init(out->hinstance);
    // create the main window with the default size
    out->hwnd = CreateWindowEx((int)NULL, "IMLAB_WINDOW", out->display_name, WS_OVERLAPPEDWINDOW,
                               0, 0, 256, 256,
                               NULL, NULL, out->hinstance, NULL);
    // check that we can create the window or return NULL
    check_null(out->hwnd, NULL, "cannot create the display window");
    // get device context
    out->hdc = CreateCompatibleDC(0);
    check_null(out->hdc, NULL, "cannot access to device context of the window");
    // show the created window
    ShowWindow(out->hwnd, SW_SHOW);
    UpdateWindow(out->hwnd);

    return out;
}

// this function clears the previously allocated memory for the image and resize the window
int window_resize(struct window_t *in, uint32_t width, uint32_t height, uint32_t bpp, uint32_t sign)  {

    // if the window is up to date, do nothing
    if(in->width == width && in->height == height && in->bpp == bpp && in->data != NULL) {
        return 1;
    }
    // free info and data pointers and set them to NULL so that no one can use it until reallocate
    if(in->binfo != NULL) {
        free(in->binfo);
        in->binfo = NULL;
    }
    // allocate memory for the info header
    in->binfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFO) + 255*sizeof(RGBQUAD));
    check_memory(in->binfo, -1);

    // fill bitmap info
    memset( &in->binfo->bmiHeader, 0, sizeof(BITMAPINFOHEADER));
    in->binfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    in->binfo->bmiHeader.biWidth = width;
    in->binfo->bmiHeader.biHeight = sign ? -height:height;
    in->binfo->bmiHeader.biPlanes = 1;
    in->binfo->bmiHeader.biBitCount = bpp;
    in->binfo->bmiHeader.biCompression = 0;
    // fill the rgb quad so that we can display 1 channel images as grayscale
    if(in->binfo->bmiHeader.biBitCount == 8) {
        uint32_t i = 0;
        for(i = 0; i < 256; i++) {
            in->binfo->bmiColors[i].rgbBlue = i;
            in->binfo->bmiColors[i].rgbGreen = i;
            in->binfo->bmiColors[i].rgbRed = i;
            in->binfo->bmiColors[i].rgbReserved = 0;
        }
    }

    // save the width and height of the window
    in->width = width;
    in->height = height;
    in->bpp = bpp;

    // check that the device context is empty
    GdiFlush();
    // get the current gdi object from the device context (this should be in->image)
    HGDIOBJ currentImage = GetCurrentObject( in->hdc, OBJ_BITMAP );
    // if there is an image inside the current device context
    if(currentImage != NULL) {
        // create a temporary bitmap
        BITMAP currentBitmap;
        // try to get the bitmap from the device
        if(GetObject(currentImage, sizeof(BITMAP), &currentBitmap) == 0) {
            // there is an object in the current device wait to finish
        }
        // delete the image from the device context
        DeleteObject( SelectObject( in->hdc, currentImage ));
    }
    // get the current window position and size
    RECT window_rect, client_rect;
    GetWindowRect(in->hwnd, &window_rect);
    GetClientRect(in->hwnd, &client_rect);

    // calculate difference between current client size and desired client size
    int client_w = client_rect.right-client_rect.left;
    int client_h = client_rect.bottom-client_rect.top;
    // compute the necessary increment for the client area
    int dx = width  - client_w;
    int dy = height - client_h;
    // compute the new position
    int window_x = window_rect.left;
    int window_y = window_rect.top;
    int window_w = window_rect.right - window_rect.left + dx;
    int window_h = window_rect.bottom - window_rect.top + dy;

    // set the window position to new width and height
    SetWindowPos(in->hwnd, 0, window_x, window_y, window_w, window_h, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
    // get the last error and analyze it
    if(GetLastError() != 0) {
        return -1;
    }
    // reallocate the display object
    in->image = SelectObject( in->hdc, CreateDIBSection(in->hdc, in->binfo, DIB_RGB_COLORS, (void**)&in->data, 0, 0));
    // check that data and image allocated correctly, or return error
    if(GetLastError() != 0) {
        return -1;
    }
    // set the size and data pointers of the display structure to the current handle
    SetWindowLongPtr(in->hwnd, GWLP_USERDATA, (LONG_PTR)in);
    // return positive number to indicate the operation is successful
    return 1;
}

// this loop waits sleep ms and handles the messages
int window_wait(struct window_t *display, int sleep) {

    DWORD time0 = GetTickCount();
    MSG msg;
    int isAnyMessageReceived = 0;

    while(1) {

        DWORD time1 = GetTickCount();
        if( abs(time1 - time0) >= sleep) {
            break;
        }
        if( PeekMessage(&msg, 0, 0, 0, PM_REMOVE) == FALSE )
        {
            Sleep(1);
            continue;
        }
        isAnyMessageReceived = 1;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    // if message is get
    if(!isAnyMessageReceived) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

void imshow(matrix_t *image, struct window_t *display) {
    // check the image
    check_null(image, );
    // check the window
    check_null(display, );

    uint32_t width_step = channels(image) * width(image);

    // update the window size if necessary
    int is_updated = window_resize(display, width(image),height(image), 8*channels(image), 1);
    // change the main window handle size
    if(is_updated >= 0) {
        // copy the data into the given data container
        if(width_step%4) {
            uint32_t row_width = width_step + (4 - (width_step % 4));
            uint32_t j;
            for(j = 0; j < height(image); j++) {
                memcpy(display->data + j*row_width, data(uint8_t, image, j*width_step), width_step);
            }
        } else {
            // if the image width is multiple of four
            memcpy(display->data, data(uint8_t, image), volume(image));
        }
        InvalidateRect(display->hwnd, 0, 0);
    }
}

#endif