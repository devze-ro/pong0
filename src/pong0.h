#ifndef PONG0_H

#include "defines.h"
#include "math.h"
#include "renderer.h"

internal void init_game(BackBuffer *back_buffer, v2u *window_dimensions);

internal void update_game(BackBuffer *back_buffer, f32 dt);

#define PONG0_H
#endif

