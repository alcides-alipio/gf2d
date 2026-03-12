#define GF2D_IMPLEMENTATION
#include "../../gf2d.h"

#include <stdlib.h>
#include <time.h>

typedef struct UserData
{
    float ballX, ballY;
    float ballVX, ballVY;
    clock_t lastTime;
} UserData;

void onDraw(gf2d_window *window, int width, int height, void *data)
{
    UserData *userData = (UserData *)data;
    int radius = 20;

    clock_t now = clock();
    float dt = (float)(now - userData->lastTime) / CLOCKS_PER_SEC;
    userData->lastTime = now;

    userData->ballX += userData->ballVX * dt;
    userData->ballY += userData->ballVY * dt;

    if (userData->ballX - radius <= 0 || userData->ballX + radius >= width)
        userData->ballVX = -userData->ballVX;
    if (userData->ballY - radius <= 0 || userData->ballY + radius >= height)
        userData->ballVY = -userData->ballVY;

    gf2d_clear(window, GF2D_WHITE);

    for (int y = (int)(userData->ballY - radius); y <= (int)(userData->ballY + radius); y++)
        for (int x = (int)(userData->ballX - radius); x <= (int)(userData->ballX + radius); x++)
        {
            int dx = x - (int)userData->ballX;
            int dy = y - (int)userData->ballY;
            if (dx*dx + dy*dy <= radius*radius)
                gf2d_draw_pixel(window, x, y, GF2D_RGB(255, 0, 0));
        }
}

int main(void)
{
    srand((unsigned int)time(NULL));

    UserData *data = malloc(sizeof(UserData));
    if (!data) return 1;

    data->ballX = 100 + rand() % 600;
    data->ballY = 100 + rand() % 400;

    data->ballVX = (float)((rand() % 200) + 100) * (rand() % 2 ? 1 : -1);
    data->ballVY = (float)((rand() % 200) + 100) * (rand() % 2 ? 1 : -1);
    data->lastTime = clock();

    gf2d_init();
    gf2d_window *window = gf2d_create_window("Random Ball", 800, 600);

    gf2d_set_draw_callback(window, onDraw, data);
    gf2d_run_window(window);

    gf2d_destroy_window(window);
    gf2d_destroy();
    free(data);

    return 0;
}