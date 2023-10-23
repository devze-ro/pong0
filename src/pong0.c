#include "pong0.h"
#include "defines.h"
#include "renderer.h"
#include "renderer.c"

global Quad g_window;
global Quad g_ball;

internal void init_game(BackBuffer *back_buffer, v2u *window_dimensions)
{
    init_back_buffer(
            back_buffer,
            window_dimensions->x,
            window_dimensions->y);

    g_window.dim.x = window_dimensions->x;
    g_window.dim.y = window_dimensions->y;
    g_window.pos.x = back_buffer->width / 2;
    g_window.pos.y = back_buffer->height / 2;
    g_ball.color.r = 0;
    g_ball.color.g = 0;
    g_ball.color.b = 0;

    g_ball.dim.x = 10;
    g_ball.dim.y = 10;
    g_ball.pos.x = back_buffer->width / 2;
    g_ball.pos.y = back_buffer->height / 2;
    g_ball.vel.x = 300;
    g_ball.vel.y = 300;
    g_ball.color.r = 255;
    g_ball.color.g = 255;
    g_ball.color.b = 255;
}

internal void update_game(BackBuffer *back_buffer, f32 dt)
{
    draw_quad(back_buffer, &g_window);

    f32 dx = g_ball.vel.x * dt;
    f32 dy = g_ball.vel.y * dt;
    f32 left_wall_distance = g_ball.pos.x - (0 + (g_ball.dim.x / 2));
    f32 right_wall_distance = back_buffer->width - 1 - (g_ball.dim.x / 2) - g_ball.pos.x;
    f32 bottom_wall_distance = g_ball.pos.y - (0 + (g_ball.dim.y / 2));
    f32 top_wall_distance = back_buffer->height - 1 - (g_ball.dim.y / 2) - g_ball.pos.y;

    if (dx < -left_wall_distance)
    {
        dx = -left_wall_distance;
        g_ball.vel.x = -g_ball.vel.x;
    }
    if (dx > right_wall_distance)
    {
        dx = right_wall_distance;
        g_ball.vel.x = -g_ball.vel.x;
    }
    if (dy < -bottom_wall_distance)
    {
        dy = -bottom_wall_distance;
        g_ball.vel.y = -g_ball.vel.y;
    }
    if (dy > top_wall_distance)
    {
        dy = top_wall_distance;
        g_ball.vel.y = -g_ball.vel.y;
    }

    g_ball.pos.x += dx;
    g_ball.pos.y += dy;
    draw_quad(back_buffer, &g_ball);
}

