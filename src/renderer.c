#include <stdlib.h>

#include "defines.h"
#include "math.h"
#include "renderer.h"
#include "physics.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "../lib/stb_truetype.h"

global stbtt_fontinfo g_font;

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
    v2 end_corner = {0};
    start_corner.x = quad->pos.x - quad->half_dim.x;
    start_corner.y = quad->pos.y - quad->half_dim.y;
    start_corner.x = start_corner.x < 0 ? 0 : start_corner.x;
    start_corner.y = start_corner.y < 0 ? 0 : start_corner.y;

    u32 start_byte_row = ((u32)start_corner.y) * buffer->pitch;
    u32 start_byte_col = ((u32)start_corner.x) * buffer->bytes_per_pixel; 

    u8 *row = (u8 *)(buffer->memory + start_byte_row + start_byte_col);

    // TODO: Add max buffer boundary check.
    f32 full_width = quad->half_dim.x * 2;
    f32 full_height = quad->half_dim.y * 2;

    for (u32 y = 0; y < full_height; ++y)
    {
        u32 *pixel = (u32 *)row;
        for (u32 x = 0; x < full_width; ++x)
        {
            *pixel++ = (quad->color.a << 24 | quad->color.r << 16 |
                    quad->color.g << 8 |
                    quad->color.b);
        }
        row += buffer->pitch;
    }
}

internal void draw_dotted_line(BackBuffer *buffer, Quad *line_quad)
{
    v2 start_corner = {0};
    start_corner.x = line_quad->pos.x - line_quad->half_dim.x;
    start_corner.y = line_quad->pos.y - line_quad->half_dim.y;

    u32 start_byte_row = ((u32)start_corner.y) * buffer->pitch;
    u32 start_byte_col = ((u32)start_corner.x) * buffer->bytes_per_pixel; 

    u8 *row = (u8 *)(buffer->memory + start_byte_row + start_byte_col);

    f32 full_width = line_quad->half_dim.x * 2;
    f32 full_height = line_quad->half_dim.y * 2;

    u32 dot_height = 20;
    u32 drawn_dot_height = 0;
    u32 dot_count = 0;
    b32 skip = 0;

    for (u32 y = 0; y < full_height; ++y)
    {
        if (!skip)
        {
            u32 *pixel = (u32 *)row;
            for (u32 x = 0; x < full_width; ++x)
            {
                *pixel++ = (line_quad->color.a << 24 | line_quad->color.r << 16 |
                        line_quad->color.g << 8 |
                        line_quad->color.b);
            }
        }
        row += buffer->pitch;
        drawn_dot_height++;
        if (drawn_dot_height == dot_height)
        {
            drawn_dot_height = 0;
            dot_count++;
            
            if (dot_count & 1)
            {
                skip = 1;
            }
            else
            {
                skip = 0;
            }
        }
    }
}

internal u8 *load_file(const char *file_path) {
    FILE *file_handle;
    long file_size = 0;

    file_handle = fopen(file_path, "rb");
    if (file_handle == NULL) {
        return 0;
    }
    fseek(file_handle, 0L, SEEK_END);
    file_size = ftell(file_handle);
    rewind(file_handle);

    u8 *buffer = (u8 *)malloc(file_size + 1);
    size_t bytes_read = fread(buffer, 1, file_size, file_handle);

    if (bytes_read != file_size) {
        printf("Error reading the file.\n");
        free(buffer);
        buffer = 0;
        fclose(file_handle);
        return 0;
    }
    buffer[file_size] = '\0';

    fclose(file_handle);
    return buffer;
}

internal void init_font()
{
    u8 *font_buffer = load_file("assets/Inconsolata.ttf");

    if (!stbtt_InitFont(&g_font, font_buffer,
                stbtt_GetFontOffsetForIndex(font_buffer,0)))
    {
        printf("Error initializing font!");
    }
}

internal void draw_text_bitmap(BackBuffer* buffer, Bitmap* bitmap)
{
    v2 start_corner = {0};
    v2 end_corner = {0};
    start_corner.x = bitmap->pos.x - (bitmap->dim.x / 2);
    start_corner.y = bitmap->pos.y - (bitmap->dim.y / 2);
    start_corner.x = start_corner.x < 0 ? 0 : start_corner.x;

    u32 start_byte_row = ((u32)start_corner.y) * buffer->pitch;
    u32 start_byte_col = ((u32)start_corner.x) * buffer->bytes_per_pixel; 

    u8 *row = (u8 *)(buffer->memory + start_byte_row + start_byte_col);

    for (u32 y = 0; y < bitmap->dim.y; ++y)
    {
        u32 *pixel = (u32 *)row;
        for (u32 x = 0; x < bitmap->dim.x; ++x)
        {
            u8 alpha = bitmap->memory[x + y * (u32)bitmap->dim.x];

            u32 bg_color = *pixel;
            u32 text_color = 0xffffff;

            u32 bg_r = (bg_color >> 16) & 0xff;
            u32 bg_g = (bg_color >> 8) & 0xff;
            u32 bg_b = bg_color & 0xff;

            u32 blended_r = (bg_r * (255 - alpha) +
                    ((text_color >> 16) & 0xff) * alpha) / 255;
            u32 blended_g = (bg_g * (255 - alpha) +
                    ((text_color >> 8) & 0xff) * alpha) / 255;
            u32 blended_b = (bg_b * (255 - alpha) +
                    (text_color & 0xff) * alpha) / 255;

            *pixel++ = (blended_r << 16) | (blended_g << 8) | blended_b;
        }
        row += buffer->pitch;
    }
}

internal void create_text_bitmap(Bitmap *bitmap, char *text, u32 font_size)
{
    f32 scale = stbtt_ScaleForPixelHeight(&g_font, font_size);

    i32 ascent, descent, line_gap;
    stbtt_GetFontVMetrics(&g_font, &ascent, &descent, &line_gap);
    ascent = (i32)(ascent * scale);
    descent = (i32)(descent * scale);

    i32 text_width = 0;
    i32 text_height = ascent - descent;

    for (u32 i = 0; text[i]; ++i)
    {
        i32 advance, lsb, kern = 0;
        stbtt_GetCodepointHMetrics(&g_font, text[i], &advance, &lsb);
        if (text[i + 1]) {
            kern = stbtt_GetCodepointKernAdvance(&g_font, text[i], text[i + 1]);
        }
        text_width += (advance + kern) * scale;
    }

    bitmap->memory = (u8 *)calloc(text_width * text_height, sizeof(u8));
    if (!bitmap->memory)
    {
        printf("Text bitmap memory allocation failed!");
    }
    bitmap->dim = (v2){ text_width , text_height };

    i32 x = 0;
    for (u32 i = 0; text[i]; ++i)
    {
        i32 advance, lsb, kern = 0;
        stbtt_GetCodepointHMetrics(&g_font, text[i], &advance, &lsb);
        if (text[i + 1]) {
            kern = stbtt_GetCodepointKernAdvance(&g_font, text[i], text[i + 1]);
        }

        i32 c_x1, c_y1, c_x2, c_y2;
        stbtt_GetCodepointBitmapBox(&g_font, text[i], scale, scale, &c_x1, &c_y1,
                &c_x2, &c_y2);

        i32 y = ascent + c_y1;

        stbtt_MakeCodepointBitmap(&g_font, bitmap->memory + (x + c_x1) +
                y * text_width, c_x2 - c_x1, c_y2 - c_y1, text_width, scale, scale,
                text[i]);

        x += (advance + kern) * scale;
    }
}

internal void display_text(BackBuffer *back_buffer, char *text, v2 ref_pos,
        v2 direction, u32 font_size)
{
    Bitmap text_bitmap = {0};
    create_text_bitmap(&text_bitmap, text, font_size);
    text_bitmap.pos = get_abs_pos_from_rel_pos(text_bitmap.dim.x,
            text_bitmap.dim.y, ref_pos, direction);
    draw_text_bitmap(back_buffer, &text_bitmap);
    free(text_bitmap.memory);
}

