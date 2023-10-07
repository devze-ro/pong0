#ifndef RENDERER_H

#include "defines.h"
#include "math.h"

typedef struct Color
{
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} Color;

typedef struct BackBuffer
{
    void *memory;
    u32 width;
    u32 height;
    u32 bytes_per_pixel;
    u32 pitch;
} BackBuffer;

typedef struct Quad
{
    v2 dim;
    v2 pos;
    Color color;
} Quad;

internal void init_back_buffer(
        BackBuffer *buffer,
        u32 buffer_width,
        u32 buffer_height);

internal void draw_quad(BackBuffer *buffer, Quad *quad);

#define RENDERER_H
#endif

