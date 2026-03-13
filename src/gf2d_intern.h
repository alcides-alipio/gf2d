#ifndef GF2D_INTERN_H_
#define GF2D_INTERN_H_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "gf2d.h"
#include <windows.h>

extern WNDCLASSW wc;

LRESULT CALLBACK WindowProcW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

gf2d_pixel *gf2d_alloc_pixels(int width, int height);
void gf2d_destroy_pixels(gf2d_pixel *pixels);

#endif