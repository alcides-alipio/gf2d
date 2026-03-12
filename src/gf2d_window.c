#include "gf2d.h"
#include "gf2d_intern.h"

struct gf2d_window
{
    HWND handle;
    int width;
    int height;

    gf2d_graphics_buffer graphicBuffer;

    void *data;
    void (*drawCallback)(gf2d_window *, int, int, void *);
};

static inline wchar_t *utf8_to_utf16(const char *str)
{
    if (!str)
        return NULL;

    int size = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    if (size == 0)
        return NULL;

    wchar_t *wstr = malloc(size * sizeof(wchar_t));
    if (!wstr)
        return NULL;

    if (MultiByteToWideChar(CP_UTF8, 0, str, -1, wstr, size) == 0)
    {
        free(wstr);
        return NULL;
    }

    return wstr;
}

LRESULT CALLBACK WindowProcW(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_CREATE)
    {
        CREATESTRUCTW *cs = (CREATESTRUCTW *)lParam;
        gf2d_window *window = (gf2d_window *)cs->lpCreateParams;
        SetWindowLongPtrW(handle, GWLP_USERDATA, (LONG_PTR)window);

        SetTimer(handle, 1, 1000 / 60, NULL);

        return 0;
    }

    gf2d_window *window = (gf2d_window *)GetWindowLongPtrW(handle, GWLP_USERDATA);
    if (!window)
        return DefWindowProcW(handle, message, wParam, lParam);

    switch (message)
    {
    case WM_TIMER:
    {
        InvalidateRect(handle, NULL, TRUE);

        return 0;
    }

    case WM_SIZE:
    {
        RECT rc;
        GetClientRect(handle, &rc);

        int newWidth = rc.right - rc.left;
        int newHeight = rc.bottom - rc.top;

        if (newWidth <= 0 || newHeight <= 0)
            return 0;

        int oldWidth = window->width;
        int oldHeight = window->height;

        gf2d_graphics_pixel *oldPixels = window->graphicBuffer.pixels;
        gf2d_graphics_pixel *newPixels = gf2d_alloc_pixels_array(newWidth, newHeight);

        if (!newPixels)
            return 0;

        int copyWidth = oldWidth < newWidth ? oldWidth : newWidth;
        int copyHeight = oldHeight < newHeight ? oldHeight : newHeight;

        for (int y = 0; y < copyHeight; y++)
        {
            memcpy(
                &newPixels[y * newWidth],
                &oldPixels[y * oldWidth],
                copyWidth * sizeof(gf2d_graphics_pixel));
        }

        gf2d_destroy_pixels_array(oldPixels);

        window->graphicBuffer.pixels = newPixels;
        window->width = newWidth;
        window->height = newHeight;

        return 0;
    }

    case WM_PAINT:
    {
        if (window && window->drawCallback)
            window->drawCallback(window, window->width, window->height, window->data);

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(handle, &ps);

        BITMAPINFO bmi = {0};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = window->width;
        bmi.bmiHeader.biHeight = -window->height;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        StretchDIBits(
            hdc,
            0, 0,
            window->width,
            window->height,
            0, 0,
            window->width,
            window->height,
            window->graphicBuffer.pixels,
            &bmi,
            DIB_RGB_COLORS,
            SRCCOPY);

        EndPaint(handle, &ps);
        return 0;
    }

    case WM_DESTROY:
        KillTimer(handle, 1);
        PostQuitMessage(0);
        return 0;

    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProcW(handle, message, wParam, lParam);
    }
}

gf2d_window *gf2d_create_window(const char *title, int width, int height)
{
    gf2d_window *window = malloc(sizeof(gf2d_window));
    if (!window)
        return NULL;

    memset(window, 0, sizeof(gf2d_window));

    wchar_t *wtitle = utf8_to_utf16(title);
    if (!wtitle)
    {
        gf2d_destroy_window(window);
        return NULL;
    }

    window->handle = CreateWindowW(
        wc.lpszClassName, wtitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        NULL, NULL,
        wc.hInstance, window);

    free(wtitle);

    if (!window->handle)
    {
        gf2d_destroy_window(window);
        return NULL;
    }

    window->width = width;
    window->height = height;

    window->graphicBuffer.pixels = gf2d_alloc_pixels_array(width, height);
    if (!window->graphicBuffer.pixels)
    {
        gf2d_destroy_window(window);
        return NULL;
    }

    return window;
}

void gf2d_destroy_window(gf2d_window *window)
{
    if (!window)
        return;

    if (window->handle)
        DestroyWindow(window->handle);

    if (window->graphicBuffer.pixels)
        gf2d_destroy_pixels_array(window->graphicBuffer.pixels);

    free(window);
    window = NULL;
}

void gf2d_run_window(gf2d_window *window)
{
    ShowWindow(window->handle, SW_SHOW);
    UpdateWindow(window->handle);

    MSG msg = {0};

    while (GetMessageW(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void gf2d_set_draw_callback(gf2d_window *window, void (*draw_callback)(gf2d_window *, int, int, void *), void *data)
{
    if (!window)
        return;

    if (!window->handle)
        return;

    window->drawCallback = draw_callback;
    window->data = data;
}

void gf2d_clear(gf2d_window *window, gf2d_color color)
{
    gf2d_graphics_pixel *pixels = window->graphicBuffer.pixels;
    int count = window->width * window->height;

    for (int i = 0; i < count; i++)
    {
        gf2d_color *dst = &pixels[i].color;

        dst->r = (dst->r * (255 - color.a) + color.r * color.a) / 255;
        dst->g = (dst->g * (255 - color.a) + color.g * color.a) / 255;
        dst->b = (dst->b * (255 - color.a) + color.b * color.a) / 255;
        dst->a = 255;
    }
}

void gf2d_draw_pixel(gf2d_window *window, int x, int y, gf2d_color color)
{
    if (!window)
        return;

    if (!window->graphicBuffer.pixels)
        return;

    if (x < 0 || y < 0)
        return;

    if (x >= window->width || y >= window->height)
        return;

    gf2d_graphics_buffer *buffer = &window->graphicBuffer;

    gf2d_graphics_pixel *dst =
        &buffer->pixels[y * window->width + x];

    gf2d_byte alpha = color.a;

    dst->color.r = (color.r * alpha + dst->color.r * (255 - alpha)) / 255;
    dst->color.g = (color.g * alpha + dst->color.g * (255 - alpha)) / 255;
    dst->color.b = (color.b * alpha + dst->color.b * (255 - alpha)) / 255;

    dst->color.a = 255;
}