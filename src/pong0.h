#ifndef PONG0_H

#include "defines.h"
#include "math.h"
#include "renderer.h"

typedef struct KeyState
{
    b32 pressed;
    b32 changed;
} KeyState;

typedef struct InputState
{
    KeyState w;
    KeyState s;
    KeyState up;
    KeyState down;
} InputState;

typedef enum Direction
{
    Up,
    Down,
    Left,
    Right
} Direction;

internal void init_game(BackBuffer *back_buffer, v2u *window_dimensions);

internal void update_game(BackBuffer *back_buffer, InputState *input_state,
        f32 dt);

internal void render_game(
        BackBuffer *back_buffer,
        InputState *input_state,
        b32 is_game_paused);

#define PONG0_H
#endif

