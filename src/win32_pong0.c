#include <stdio.h>
#include <stdint.h>
#include <windows.h>
#include <winuser.h>

#include "defines.h"
#include "pong0.h"
#include "pong0.c"

global b32 g_is_game_running;
global b32 g_has_game_started;
global b32 g_is_game_paused;
global b32 g_is_game_over;
global b32 g_is_single_player = 1;
global b32 g_is_sound_muted;
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

internal void set_key_changed_state(KeyState *key_state, b32 pressed_now)
{
    if (key_state->pressed != pressed_now)
    {
        key_state->pressed = pressed_now;
        key_state->changed = 1;
    }
}

internal void process_messages(InputState *input_state)
{
    MSG msg;
    while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
        u32 key_code = msg.wParam;
        switch(msg.message)
        {
            case WM_KEYDOWN:
            case WM_KEYUP:
                b32 pressed_now = (msg.message == WM_KEYDOWN);

                if (g_has_game_started)
                {
                    if (!g_is_game_paused && !g_is_game_over)
                    {
                        if (!g_is_single_player)
                        {
                            if (key_code == 'W')
                            {
                                set_key_changed_state(&input_state->w,
                                        pressed_now);
                            }
                            if (key_code == 'S')
                            {
                                set_key_changed_state(&input_state->s,
                                        pressed_now);
                            }
                        }
                        if (key_code == VK_UP)
                        {
                            set_key_changed_state(&input_state->up,
                                    pressed_now);
                        }
                        if (key_code == VK_DOWN)
                        {
                            set_key_changed_state(&input_state->down,
                                    pressed_now);
                        }
                    }
                    if (key_code == 'P' && pressed_now)
                    {
                        if (g_is_game_paused)
                        {
                            g_is_game_paused = 0;
                        }
                        else if (g_has_game_started && !g_is_game_paused)
                        {
                            g_is_game_paused = 1;
                        }
                    }

                    if (key_code == 'R' && g_is_game_over)
                    {
                        g_is_game_over = 0;
                        init_dynamic_props(&g_back_buffer);
                    }
                }
                else
                {
                    if (key_code == '1' && pressed_now)
                    {
                        g_is_single_player = 1;
                        g_has_game_started = 1;
                    }
                    if (key_code == '2' && pressed_now)
                    {
                        g_is_single_player = 0;
                        g_has_game_started = 1;
                    }
                }

                if (key_code == 'M' && pressed_now)
                {
                    g_is_sound_muted = g_is_sound_muted ? 0 : 1;
                }

                break;

            default:
                TranslateMessage(&msg);
                DispatchMessage(&msg);
        }
    }
}

DWORD WINAPI win32_init_audio(LPVOID lpParam)
{
    init_audio();
    return 0;
}

int WINAPI WinMain(
        HINSTANCE instance,
        HINSTANCE prev_instance,
        LPSTR command_line,
        int show_code)
{
    printf("Welcome to Pong0!\n");

    // TODO (devze_ro): Initializing audio in a separate thread else it slows
    // the game initialization. Is asynchronous loading required for such small
    // audio files?
    HANDLE thread = CreateThread(NULL, 0, win32_init_audio, NULL, 0, NULL);

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

    RECT window_rect = {0, 0, 960, 720};
    AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, FALSE);
    HWND window_handle = CreateWindowEx(
            0,
            window_class.lpszClassName,
            "pong0",
            WS_OVERLAPPEDWINDOW|WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            window_rect.right - window_rect.left,
            window_rect.bottom - window_rect.top,
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

    DEVMODE devMode;
    devMode.dmSize = sizeof(devMode);

    if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode)) {
        printf("Current screen refresh rate: %d Hz\n",
                devMode.dmDisplayFrequency);
    } else {
        printf("Error retrieving display settings\n");
    }

    f32 target_seconds_per_frame = 1.0f / devMode.dmDisplayFrequency;
    timeBeginPeriod(1);
    g_is_game_running = 1;

    InputState input_state = {0};

    while (g_is_game_running)
    {
        LARGE_INTEGER start_counter;
        QueryPerformanceCounter(&start_counter);

        process_messages(&input_state);

        if (g_has_game_started && !g_is_game_paused && !g_is_game_over)
        {
            update_game(&g_back_buffer, &input_state, target_seconds_per_frame,
                    &g_is_game_over, g_is_single_player, g_is_sound_muted);
        }

        render_game(&g_back_buffer, &input_state, g_has_game_started,
                g_is_game_paused, g_is_game_over);
        HDC device_context = GetDC(window_handle);
        blit(device_context, window_handle, &g_back_buffer);
        ReleaseDC(window_handle, device_context);

        LARGE_INTEGER end_counter;
        QueryPerformanceCounter(&end_counter);
        LARGE_INTEGER counter_frequency;
        QueryPerformanceFrequency(&counter_frequency);

        f32 tick_count = (f32)(end_counter.QuadPart - start_counter.QuadPart);
        f32 elapsed_seconds =
            (f32)(end_counter.QuadPart - start_counter.QuadPart) /
            (f32)counter_frequency.QuadPart;

        if (elapsed_seconds < target_seconds_per_frame)
        {
            DWORD remaining_time_in_ms = (DWORD)(
                    (target_seconds_per_frame - elapsed_seconds) * 1000.0f - 1);

            if (remaining_time_in_ms > 0)
            {
                Sleep(remaining_time_in_ms);
            }

            while (elapsed_seconds < target_seconds_per_frame)
            {
                QueryPerformanceCounter(&end_counter);

                tick_count = (f32)(end_counter.QuadPart - start_counter.QuadPart);
                elapsed_seconds = tick_count / (f32)counter_frequency.QuadPart;
            }
            printf("frame duration in seconds: %.4f\n", elapsed_seconds);
        }
    }

    return 0;
}

