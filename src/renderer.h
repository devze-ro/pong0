#ifndef RENDERER_H

#include "defines.h"
#include "math.h"

typedef struct BackBuffer
{
    void *memory;
    u32 width;
    u32 height;
    u32 bytes_per_pixel;
    u32 pitch;
} BackBuffer;

typedef struct Bitmap
{
    u8 *memory;
    v2 dim;
    v2 pos;
} Bitmap;

typedef struct Color
{
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} Color;

typedef struct Quad
{
    v2 half_dim;
    v2 pos;
    v2 vel;
    Color color;
} Quad;

typedef struct QuadMinMax
{
    v2 min;
    v2 max;
} QuadMinMax;

internal void init_back_buffer(
        BackBuffer *buffer,
        u32 buffer_width,
        u32 buffer_height);

internal void draw_quad(BackBuffer *buffer, Quad *quad);

internal void draw_dotted_line(BackBuffer *buffer, Quad *line_quad);

internal void init_font();

internal void display_text(BackBuffer *back_buffer, char *text, v2 pos,
        v2 margin, u32 font_size);

#define RENDERER_H
#endif

