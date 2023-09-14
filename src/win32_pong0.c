#include <stdio.h>
#include <windows.h>

LRESULT CALLBACK window_message_handler(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR lpCmdLine,
  int nShowCmd)
{
    printf("Welcome to Pong0!");

    WNDCLASS window_class = {0};
    window_class.hInstance = hInstance;
    window_class.lpfnWndProc = window_message_handler;
    window_class.lpszClassName = "Pong_Window";

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
        hInstance,
        0
    );

    if (!window_handle)
    {
        MessageBox(0, "Window creation failed!", "Error", MB_ICONERROR);
        return 1;
    }

    MSG message = {0};
    while (GetMessage(&message, 0, 0, 0))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    return 0;
}

