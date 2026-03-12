#include "gf2d.h"
#include "gf2d_intern.h"

WNDCLASSW wc = {0};

void gf2d_init(void)
{
    wc.lpfnWndProc = WindowProcW;
    wc.lpszClassName = L"GUICWindowClass";
    wc.hInstance = GetModuleHandleW(NULL);
    wc.hCursor = LoadCursorW(NULL, MAKEINTRESOURCEW(32512));
    wc.hbrBackground = NULL;

    RegisterClassW(&wc);

    SetProcessDPIAware();
}

void gf2d_destroy(void)
{
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
}