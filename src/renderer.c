#include <stdlib.h>

#include "defines.h"
#include "math.h"
#include "renderer.h"

internal void init_back_buffer(
        BackBuffer *buffer,
        u32 buffer_width,
        u32 buffer_height)
{
    buffer->width = buffer_width;
    buffer->height = buffer_height;
    buffer->bytes_per_pixel = 4;
    buffer->pitch = buffer->width * buffer->bytes_per_pixel;

    u64 buffer_size = buffer->width * buffer->height * buffer->bytes_per_pixel;
    buffer->memory = malloc(buffer_size);
}

internal void draw_quad(BackBuffer *buffer, Quad *quad)
{
    v2 start_corner = {0};
    start_corner.x = quad->pos.x - quad->half_dim.x;
    start_corner.y = quad->pos.y - quad->half_dim.y;

    u32 start_byte_row = ((u32)start_corner.y) * buffer->pitch;
    u32 start_byte_col = ((u32)start_corner.x) * buffer->bytes_per_pixel; 

    u8 *row = (u8 *)(buffer->memory + start_byte_row + start_byte_col);
    u8 alpha = 255;

    f32 full_width = quad->half_dim.x * 2;
    f32 full_height = quad->half_dim.y * 2;

    for (u32 y = 0; y < full_height; ++y)
    {
        u32 *pixel = (u32 *)row;
        for (u32 x = 0; x < full_width; ++x)
        {
            *pixel++ = (alpha << 24 | quad->color.r << 16 |
                    quad->color.g << 8 |
                    quad->color.b);
        }
        row += buffer->pitch;
    }
}

