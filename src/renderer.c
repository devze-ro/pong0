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
    f32 half_width = quad->dim.x / 2;
    f32 half_height = quad->dim.y / 2;

    v2 start_corner = {0};
    start_corner.x = quad->pos.x - half_width;
    start_corner.y = quad->pos.y - half_height;

    u32 start_byte_row = ((u32)start_corner.y) * buffer->pitch;
    u32 start_byte_col = ((u32)start_corner.x) * buffer->bytes_per_pixel; 

    u8 *row = (u8 *)(buffer->memory + start_byte_row + start_byte_col);
    u8 alpha = 255;
    for (u32 y = 0; y < quad->dim.y; ++y)
    {
        u32 *pixel = (u32 *)row;
        for (u32 x = 0; x < quad->dim.x; ++x)
        {
            quad->color.r += 1;
            quad->color.g += 2;
            quad->color.b -= 4;
            *pixel++ = (alpha << 24 | quad->color.r << 16 |
                    quad->color.g << 8 |
                    quad->color.b);
        }
        row += buffer->pitch;
    }
}

