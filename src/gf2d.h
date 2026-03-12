#ifndef GF2D_H_
#define GF2D_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define GF2D_ARGB(a, r, g, b) \
    ((gf2d_color){b, g, r, a})
#define GF2D_RGBA(r, g, b, a) \
    ((gf2d_color){b, g, r, a})
#define GF2D_RGB(r, g, b) \
    ((gf2d_color){b, g, r, 255})

#define GF2D_WHITE GF2D_RGB(255, 255, 255)
#define GF2D_BLACK GF2D_RGB(0, 0, 0)
#define GF2D_RED GF2D_RGB(255, 0, 0)
#define GF2D_GREEN GF2D_RGB(0, 255, 0)
#define GF2D_BLUE GF2D_RGB(0, 0, 255)

    typedef struct gf2d_window gf2d_window;

    typedef unsigned char gf2d_byte;
    typedef unsigned int gf2d_uint32;

    typedef struct gf2d_color
    {
        gf2d_byte b, g, r, a;
    } gf2d_color;

    void gf2d_init(void);
    void gf2d_destroy(void);

    gf2d_window *gf2d_create_window(const char *title, int width, int height);
    void gf2d_destroy_window(gf2d_window *window);

    void gf2d_run_window(gf2d_window *window);

    void gf2d_set_draw_callback(gf2d_window *window, void (*draw_callback)(gf2d_window *, int, int, void *), void *data);

    void gf2d_draw_pixel(gf2d_window *window, int x, int y, gf2d_color color);
    void gf2d_clear(gf2d_window *window, gf2d_color color);
#ifdef __cplusplus
}
#endif

#endif