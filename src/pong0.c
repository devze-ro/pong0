#include "pong0.h"
#include "defines.h"
#include "renderer.h"
#include "renderer.c"

global Quad g_quad;

internal void init_game(BackBuffer *back_buffer, v2u *window_dimensions)
{
    init_back_buffer(
            back_buffer,
            window_dimensions->x,
            window_dimensions->y);

    g_quad.dim.x = 256;
    g_quad.dim.y = 256;
    g_quad.pos.x = back_buffer->width / 2;
    g_quad.pos.y = back_buffer->height / 2;
    g_quad.color.r = 0;
    g_quad.color.g = 0;
    g_quad.color.b = 255;
}

internal void update_game(BackBuffer *back_buffer)
{
    g_quad.color.r += 1;
    g_quad.color.g += 2;
    g_quad.color.b -= 4;
    draw_quad(back_buffer, &g_quad);
}

