#include <stdio.h>
#include <stdint.h>
#include <windows.h>

#include "pong0.h"
#include "pong0.c"

global b32 g_is_game_running;
global BackBuffer g_back_buffer;
global BITMAPINFO g_bitmap_info;

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
            &g_bitmap_info,
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
            0);

    if (!window_handle)
    {
        MessageBox(0, "Window creation failed!", "Error", MB_ICONERROR);
        return 1;
    }

    v2u window_dimensions = get_window_dimensions(window_handle);
    init_game(&g_back_buffer ,&window_dimensions);

    g_bitmap_info.bmiHeader.biSize = sizeof(g_bitmap_info.bmiHeader);
    g_bitmap_info.bmiHeader.biWidth = g_back_buffer.width;
    g_bitmap_info.bmiHeader.biHeight = -g_back_buffer.height;
    g_bitmap_info.bmiHeader.biPlanes = 1;
    g_bitmap_info.bmiHeader.biBitCount = 32;
    g_bitmap_info.bmiHeader.biCompression = BI_RGB;

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

        update_game(&g_back_buffer);
        blit(device_context, window_handle, &g_back_buffer);

        ReleaseDC(window_handle, device_context);
    }

    return 0;
}

