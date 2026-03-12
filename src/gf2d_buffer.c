#include "gf2d.h"
#include "gf2d_intern.h"

gf2d_graphics_pixel *gf2d_alloc_pixels_array(int width, int height)
{
    if (width <= 0 || height <= 0)
        return NULL;

    size_t count = (size_t)width * (size_t)height;

    gf2d_graphics_pixel *pixels = calloc(count, sizeof(gf2d_graphics_pixel));
    if (!pixels)
        return NULL;

    for (size_t i = 0; i < count; i++)
    {
        pixels[i].color.r = 255;
        pixels[i].color.g = 255;
        pixels[i].color.b = 255;
        pixels[i].color.a = 255;
    }

    return pixels;
}

void gf2d_destroy_pixels_array(gf2d_graphics_pixel *pixels)
{
    if (!pixels)
        return;

    free(pixels);
}