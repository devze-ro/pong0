#include "pong0.h"
#include "defines.h"
#include "renderer.h"
#include "renderer.c"

global Quad g_window;
global Quad g_ball;
global Quad g_left_paddle;
global Quad g_right_paddle;
global b32 g_is_ball_hit;

internal void init_game(BackBuffer *back_buffer, v2u *window_dimensions)
{
    init_back_buffer(
            back_buffer,
            window_dimensions->x,
            window_dimensions->y);

    v2 back_buffer_center = (v2){
        back_buffer->width * 0.5,
            back_buffer->height * 0.5
    };

    g_window.half_dim = (v2){ back_buffer_center.x, back_buffer_center.y };
    g_window.pos = (v2){ back_buffer_center.x, back_buffer_center.y };
    g_window.color = (Color){ 0, 0, 0, 255 };

    g_ball.half_dim = (v2){ 10.0, 10.0 };
    g_ball.pos = (v2){ back_buffer_center.x, back_buffer_center.y };
    g_ball.vel = (v2){ 400.0, 400.0 };
    g_ball.color = (Color){ 255, 255, 255, 255 };

    g_left_paddle.half_dim = (v2){ 10.0, 50.0 };
    g_left_paddle.pos = (v2){ 50, back_buffer_center.y };
    g_left_paddle.vel = (v2){ 300.0, 300.0 };
    g_left_paddle.color = (Color){ 255, 255, 255, 255 };

    g_right_paddle.half_dim = (v2){ 10.0, 50.0 };
    g_right_paddle.pos = (v2){ back_buffer->width - 50, back_buffer_center.y };
    g_right_paddle.vel = (v2){ 300.0, 300.0 };
    g_right_paddle.color = (Color){ 255, 255, 255, 255 };
}

internal QuadMinMax get_quad_min_max(Quad *quad)
{
    QuadMinMax result = {0};
    result.min = (v2){
        quad->pos.x - quad->half_dim.x,
            quad->pos.y - quad->half_dim.y };
    result.max = (v2){
        quad->pos.x + quad->half_dim.x,
            quad->pos.y + quad->half_dim.y };
    return result;
}

internal b32 is_point_inside_quad(v2 *point, Quad *quad)
{
    QuadMinMax min_max = get_quad_min_max(quad);
    return (point->x >= min_max.min.x &&
            point->x <= min_max.max.x &&
            point->y >= min_max.min.y &&
            point->y <= min_max.max.y);
}

internal Quad minkowski_sum(Quad *a, Quad *b)
{
    Quad result = {0};
    result.pos = (v2){ a->pos.x, a->pos.y };
    result.half_dim = (v2){ a->half_dim.x + b->half_dim.x,
        a->half_dim.y + b->half_dim.y };
    return result;
}

internal void handle_paddle_ball_collision(Quad *paddle, v2 *ball_dpos, f32 dt)
{
    v2 ball_next_pos = (v2){
        g_ball.pos.x + ball_dpos->x,
            g_ball.pos.y + ball_dpos->y };
    Quad sum_quad = minkowski_sum(paddle, &g_ball);

    if (is_point_inside_quad(&ball_next_pos, &sum_quad))
    {
        g_is_ball_hit = 1;
        g_ball.vel.x = -g_ball.vel.x;
        if ((g_ball.pos.y < paddle->pos.y && g_ball.vel.y > 0) ||
                (g_ball.pos.y > paddle->pos.y && g_ball.vel.y < 0))
        {
            g_ball.vel.y = -g_ball.vel.y;
        }
        ball_dpos->x = g_ball.vel.x * dt;
        ball_dpos->y = g_ball.vel.y * dt;
    }
}

internal void wall_check_quad_position_change(
        Quad *quad,
        BackBuffer *back_buffer,
        v2 *dpos)
{
    QuadMinMax min_max = get_quad_min_max(quad);
    f32 left_wall_distance = min_max.min.x;
    f32 right_wall_distance = back_buffer->width - 1 - min_max.max.x;
    f32 top_wall_distance = min_max.min.y;
    f32 bottom_wall_distance = back_buffer->height - 1 - min_max.max.y;

    if (quad->vel.x < 0 && dpos->x < -left_wall_distance)
    {
        dpos->x = -left_wall_distance;
        quad->vel.x = -quad->vel.x;
    }
    if (quad->vel.x > 0 && dpos->x > right_wall_distance)
    {
        dpos->x = right_wall_distance;
        quad->vel.x = -quad->vel.x;
    }
    if (quad->vel.y < 0 && dpos->y < -top_wall_distance)
    {
        dpos->y = -top_wall_distance;
        quad->vel.y = -quad->vel.y;
    }
    if (quad->vel.y > 0 && dpos->y > bottom_wall_distance)
    {
        dpos->y = bottom_wall_distance;
        quad->vel.y = -quad->vel.y;
    }
}

internal void update_paddle_position(
        Quad *paddle,
        Direction direction,
        BackBuffer *back_buffer,
        f32 dt)
{
    if (direction == Up && paddle->vel.y > 0 ||
            direction == Down && paddle->vel.y < 0)
    {
        paddle->vel.y = -paddle->vel.y;
    }
    v2 dpos = (v2){ 0, paddle->vel.y * dt };
    wall_check_quad_position_change(
            paddle, back_buffer, &dpos);

    paddle->pos.y += dpos.y;
}

internal void update_game(
        BackBuffer *back_buffer, 
        InputState *input_state, 
        f32 dt)
{
    g_is_ball_hit = 0;
    v2 ball_dpos = (v2){ g_ball.vel.x * dt, g_ball.vel.y * dt };

    handle_paddle_ball_collision(&g_left_paddle, &ball_dpos, dt);
    handle_paddle_ball_collision(&g_right_paddle, &ball_dpos, dt);

    if (!g_is_ball_hit)
    {
        wall_check_quad_position_change(&g_ball, back_buffer, &ball_dpos);
    }

    g_ball.pos.x += ball_dpos.x;
    g_ball.pos.y += ball_dpos.y;

    if (input_state->w.pressed || input_state->w.changed)
    {
        update_paddle_position(&g_left_paddle, Up, back_buffer, dt);
    }
    if (input_state->s.pressed || input_state->s.changed)
    {
        update_paddle_position(&g_left_paddle, Down, back_buffer, dt);
    }
    if (input_state->up.pressed || input_state->up.changed)
    {
        update_paddle_position(&g_right_paddle, Up, back_buffer, dt);
    }
    if (input_state->down.pressed || input_state->down.changed)
    {
        update_paddle_position(&g_right_paddle, Down, back_buffer, dt);
    }

    draw_quad(back_buffer, &g_window);
    draw_quad(back_buffer, &g_ball);
    draw_quad(back_buffer, &g_left_paddle);
    draw_quad(back_buffer, &g_right_paddle);

    input_state->w.changed = 0;
    input_state->s.changed = 0;
    input_state->up.changed = 0;
    input_state->down.changed = 0;
}

