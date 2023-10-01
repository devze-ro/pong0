#include <stdio.h>
#include <stdint.h>
#include <windows.h>

#define global static
#define local static
#define internal static

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

typedef float f32;
typedef double f64;

typedef int b32;
typedef char b8;

typedef size_t sz;

typedef uintptr_t up;
typedef intptr_t ip;

typedef struct v2
{
    f32 x;
    f32 y;
} v2;

typedef struct v2u
{
    u32 x;
    u32 y;
} v2u;

typedef struct v3
{
    f32 x;
    f32 y;
    f32 z;
} v3;

typedef struct v3u
{
    u32 x;
    u32 y;
    u32 z;
} v3u;

typedef struct v4
{
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} v4;

typedef struct Color
{
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} Color;

typedef struct Quad
{
    v2 dim;
    v2 pos;
    Color color;
} Quad;

typedef struct BackBuffer
{
    void *memory;
    BITMAPINFO info;
    u32 width;
    u32 height;
    u32 bytes_per_pixel;
    u32 pitch;
} BackBuffer;

global b32 g_is_game_running;
global BackBuffer g_back_buffer;

internal void init_back_buffer(
        BackBuffer *buffer,
        u32 buffer_width,
        u32 buffer_height)
{
    buffer->width = buffer_width;
    buffer->height = buffer_height;
    buffer->bytes_per_pixel = 4;
    buffer->pitch = buffer->width * buffer->bytes_per_pixel;
    buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);
    buffer->info.bmiHeader.biWidth = buffer->width;
    buffer->info.bmiHeader.biHeight = -buffer->height;
    buffer->info.bmiHeader.biPlanes = 1;
    buffer->info.bmiHeader.biBitCount = 32;
    buffer->info.bmiHeader.biCompression = BI_RGB;

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

    u32 start_byte_row = ((u32)start_corner.y - 1) * buffer->pitch;
    u32 start_byte_col = ((u32)start_corner.x - 1) * buffer->bytes_per_pixel; 

    u8 *row = (u8 *)(buffer->memory + start_byte_row + start_byte_col);
    for (u32 y = 0; y < quad->dim.y; ++y)
    {
        u32 *pixel = (u32 *)row;
        for (u32 x = 0; x < quad->dim.x; ++x)
        {
            quad->color.r += 1;
            quad->color.g += 2;
            quad->color.b -= 4;
            *pixel++ = (quad->color.r << 16 |
                    quad->color.g << 8 |
                    quad->color.b);
        }
        row += buffer->pitch;
    }
}

internal v2u get_window_dimensions(HWND window_handle)
{
    v2u result;
    RECT client_rect;
    GetClientRect(window_handle, &client_rect);
    result.x = client_rect.right - client_rect.left;
    result.y = client_rect.bottom - client_rect.top;

    return result;
}

internal void blit(HDC device_context, HWND window_handle, BackBuffer *buffer)
{
    v2u window_dimensions = get_window_dimensions(window_handle);
    StretchDIBits(
            device_context,
            0, 0, buffer->width, buffer->height,
            0, 0, window_dimensions.x, window_dimensions.y,
            buffer->memory,
            &buffer->info,
            DIB_RGB_COLORS,
            SRCCOPY);
}

LRESULT CALLBACK window_message_handler(
    HWND window_handle,
    UINT msg,
    WPARAM w_param,
    LPARAM l_param)
{
    switch (msg)
    {
        case WM_CLOSE:
        case WM_DESTROY:
            g_is_game_running = 0;
            break;
        case WM_PAINT:
            PAINTSTRUCT paint;
            HDC device_context = BeginPaint(window_handle, &paint);
            blit(device_context, window_handle, &g_back_buffer);
            EndPaint(window_handle, &paint);
            break;
        default:
            return DefWindowProc(window_handle, msg, w_param, l_param);
    }

    return 0;
}

int WINAPI WinMain(
  HINSTANCE instance,
  HINSTANCE prev_instance,
  LPSTR command_line,
  int show_code)
{
    printf("Welcome to Pong0!");

    WNDCLASS window_class = {0};
    window_class.style = CS_HREDRAW|CS_VREDRAW;
    window_class.hInstance = instance;
    window_class.lpfnWndProc = window_message_handler;
    window_class.lpszClassName = "pong_window";
    if (!RegisterClass(&window_class))
    {
        MessageBox(0, "Window registration failed!", "Error", MB_ICONERROR);
        return 1;
    }

    HWND window_handle = CreateWindowEx(
        0,
        window_class.lpszClassName,
        "pong0",
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        instance,
        0
    );

    if (!window_handle)
    {
        MessageBox(0, "Window creation failed!", "Error", MB_ICONERROR);
        return 1;
    }

    v2u window_dimensions = get_window_dimensions(window_handle);

    init_back_buffer(
            &g_back_buffer,
            window_dimensions.x,
            window_dimensions.y);

    Quad quad = {0};
    quad.dim.x = 256;
    quad.dim.y = 256;
    quad.pos.x = window_dimensions.x / 2;
    quad.pos.y = window_dimensions.y / 2;
    quad.color.r = 0;
    quad.color.g = 0;
    quad.color.b = 255;

    g_is_game_running = 1;

    while (g_is_game_running)
    {
        MSG message = {0};
        if (PeekMessage(&message, window_handle, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        HDC device_context = GetDC(window_handle);

        quad.color.r += 1;
        quad.color.g += 2;
        quad.color.b -= 4;
        draw_quad(&g_back_buffer, &quad);
        blit(device_context, window_handle, &g_back_buffer);

        ReleaseDC(window_handle, device_context);
    }

    return 0;
}

