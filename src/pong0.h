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

internal void init_dynamic_props(BackBuffer *back_buffer);

internal void init_game(BackBuffer *back_buffer, v2u *window_dimensions);

internal void update_game(
        BackBuffer *back_buffer, 
        InputState *input_state, 
        f32 dt,
        b32 *is_game_over,
        b32 is_single_player,
        b32 is_audio_muted);

internal void render_game(
        BackBuffer *back_buffer,
        InputState *input_state,
        b32 has_game_started,
        b32 is_game_paused,
        b32 is_game_over);

#define PONG0_H
#endif

