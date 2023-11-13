#include "pong0.h"
#include "defines.h"
#include "renderer.h"
#include "physics.h"
#include <stdio.h>
#include <stdlib.h>

#include "renderer.c"
#include "physics.c"

global Quad g_window;
global Quad g_top_wall;
global Quad g_bottom_wall;
global Quad g_dotted_line;
global Quad g_ball;
global Quad g_left_paddle;
global Quad g_right_paddle;
global b32 g_is_ball_hit;
global b32 g_is_ball_paused;
global f32 g_ball_pause_duration;
global u32 g_left_score;
global u32 g_right_score;

internal void init_game(BackBuffer *back_buffer, v2u *window_dimensions)
{
    init_back_buffer(
            back_buffer,
            window_dimensions->x,
            window_dimensions->y);

    init_font();

    v2 back_buffer_center = (v2){
        back_buffer->width * 0.5,
            back_buffer->height * 0.5
    };

    g_window.half_dim = (v2){ back_buffer_center.x, back_buffer_center.y };
    g_window.pos = (v2){ back_buffer_center.x, back_buffer_center.y };
    g_window.color = (Color){ 0, 0, 0, 255 };

    g_top_wall.half_dim = (v2){ back_buffer_center.x, 10.0 };
    g_top_wall.pos = (v2){ back_buffer_center.x, 10.0 };
    g_top_wall.color = (Color){ 255, 255, 255, 255 };

    g_bottom_wall.half_dim = (v2){ back_buffer_center.x, 10.0 };
    g_bottom_wall.pos = (v2){ back_buffer_center.x, back_buffer->height - 10.0 };
    g_bottom_wall.color = (Color){ 255, 255, 255, 255 };

    g_dotted_line.half_dim = (v2){ 10.0, back_buffer_center.y };
    g_dotted_line.pos = (v2){ back_buffer_center.x, back_buffer_center.y };
    g_dotted_line.color = (Color){ 255, 255, 255, 255 };

    g_ball.half_dim = (v2){ 10.0, 10.0 };
    g_ball.pos = (v2){ back_buffer_center.x, back_buffer_center.y };
    g_ball.vel = (v2){ 400.0, 400.0 };
    g_ball.color = (Color){ 255, 255, 255, 255 };

    g_left_paddle.half_dim = (v2){ 10.0, 50.0 };
    g_left_paddle.pos = (v2){ 50.0, back_buffer_center.y };
    g_left_paddle.vel = (v2){ 300.0, 300.0 };
    g_left_paddle.color = (Color){ 255, 255, 255, 255 };

    g_right_paddle.half_dim = (v2){ 10.0, 50.0 };
    g_right_paddle.pos = (v2){ back_buffer->width - 50.0, back_buffer_center.y };
    g_right_paddle.vel = (v2){ 300.0, 300.0 };
    g_right_paddle.color = (Color){ 255, 255, 255, 255 };

    g_ball_pause_duration = 0.5f;
    g_is_ball_paused = 0;
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
    f32 top_wall_distance = min_max.min.y - 20;
    f32 bottom_wall_distance = back_buffer->height - min_max.max.y - 20;

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
    char left_score_str[12];
    sprintf(left_score_str, "%d", g_left_score);
    char right_score_str[12];
    sprintf(right_score_str, "%d", g_right_score);
    if (g_is_ball_paused)
    {
        g_ball_pause_duration -= dt;
        if (g_ball_pause_duration <= 0)
        {
            g_is_ball_paused = 0;
            g_ball_pause_duration = 0.5;
        }
    }
    else
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

        Quad window_ball_sum = minkowski_sum(&g_window, &g_ball);
        if (!is_point_inside_quad(&g_ball.pos, &window_ball_sum))
        {
            if (g_ball.pos.x > back_buffer->width)
            {
                g_left_score++;
                sprintf(left_score_str, "%d", g_left_score);
                printf("player 1 score = %d\n", g_left_score);
            }
            if (g_ball.pos.x < 0)
            {
                g_right_score++;
                sprintf(right_score_str, "%d", g_right_score);
                printf("player 2 score = %d\n", g_right_score);
            }
            f32 ball_pos_max = 2 * (g_window.half_dim.y - g_bottom_wall.half_dim.y -
                    g_ball.half_dim.y);
            f32 ball_pos_min = 2 * (g_top_wall.half_dim.y + g_ball.half_dim.y);

            g_ball.pos.x = g_window.pos.x;
            g_ball.pos.y = (rand() % (i32)(ball_pos_max - ball_pos_min + 1)) +
                ball_pos_min;
            g_is_ball_paused = 1;
        }
    }

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
    draw_dotted_line(back_buffer, &g_dotted_line);
    display_text(back_buffer, left_score_str, (v2){ g_window.half_dim.x - 10,
            (g_top_wall.half_dim.y * 2) }, (v2){ -1, 1 }, 80);
    display_text(back_buffer, right_score_str, (v2){ g_window.half_dim.x + 10,
            (g_top_wall.half_dim.y * 2) }, (v2){ 1, 1 }, 80);
    draw_quad(back_buffer, &g_top_wall);
    draw_quad(back_buffer, &g_bottom_wall);
    draw_quad(back_buffer, &g_ball);
    draw_quad(back_buffer, &g_left_paddle);
    draw_quad(back_buffer, &g_right_paddle);

    input_state->w.changed = 0;
    input_state->s.changed = 0;
    input_state->up.changed = 0;
    input_state->down.changed = 0;
}

