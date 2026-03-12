#ifndef GF2D_INTERN_H_
#define GF2D_INTERN_H_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "gf2d.h"
#include <windows.h>

typedef struct gf2d_graphics_pixel
{
    gf2d_color color;
} gf2d_graphics_pixel;

typedef struct gf2d_graphics_buffer
{
    gf2d_graphics_pixel *pixels;
} gf2d_graphics_buffer;

extern WNDCLASSW wc;

LRESULT CALLBACK WindowProcW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

gf2d_graphics_pixel *gf2d_alloc_pixels_array(int width, int height);
void gf2d_destroy_pixels_array(gf2d_graphics_pixel *pixels);

#endif