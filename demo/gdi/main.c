#define GF2D_IMPLEMENTATION
#include "../../gf2d.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdlib.h>
#include <time.h>

typedef struct UserData
{
    gf2d_image images[10];
} UserData;

void onDraw(gf2d_window *window, int width, int height, void *data)
{
    UserData *userData = (UserData *)data;

    int centerX = width / 2;
    int centerY = height / 2;

    int imagePosX1 = centerX - (userData->images[0].width / 2);
    int imagePosY1 = centerY - (userData->images[0].height / 2);

    gf2d_clear(window, GF2D_WHITE);
    if (userData->images[0].pixels)
        gf2d_draw_image(window, &userData->images[0], imagePosX1, imagePosY1);
}

gf2d_image getImage(const char *path)
{
    gf2d_image img = {0};

    if (!path)
        return img;

    int width, height, channels;

    stbi_uc *image = stbi_load("capivara.png", &width, &height, &channels, 4);
    if (!image)
        return img;

    gf2d_pixel *pixels = calloc(width * height, sizeof(gf2d_pixel));
    if (!pixels)
    {
        stbi_image_free(image);
        return img;
    }

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int index = (y * width + x) * 4;

            gf2d_byte r = image[index + 0];
            gf2d_byte g = image[index + 1];
            gf2d_byte b = image[index + 2];
            gf2d_byte a = image[index + 3];

            pixels[y * width + x].color = GF2D_RGBA(r, g, b, a);
        }
    }

    img.width = width;
    img.height = height;
    img.pixels = pixels;

    stbi_image_free(image);

    return img;
}

int main(void)
{
    UserData *data = malloc(sizeof(UserData));
    if (!data)
        return 1;

    data->images[0] = getImage("capivara.png");

    gf2d_init();
    gf2d_window *window = gf2d_create_window("Random Ball", 800, 600);

    gf2d_set_draw_callback(window, onDraw, data);
    gf2d_run_window(window);

    gf2d_destroy_window(window);
    gf2d_destroy();

    for (int i = 0; i < sizeof(data->images) / sizeof(data->images[0]); i++)
    {
        if (data->images[i].pixels)
            free(data->images[i].pixels);
    }

    free(data);

    return 0;
}