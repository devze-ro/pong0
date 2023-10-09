#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "pong0.h"
#include "pong0.c"

int main()
{
    XEvent event;

    Display* display = XOpenDisplay(NULL);
    if (!display)
    {
        printf("Cannot open X display\n");
        return 1;
    }

    u32 screen_num = DefaultScreen(display);
    Window root = RootWindow(display, screen_num);
    v2u window_dimensions = {960, 720};

    Window window = XCreateSimpleWindow(
            display, root, 0, 0, window_dimensions.x, window_dimensions.y,
            0, 0, 0);

    XStoreName(display, window, "pong0");

    XSelectInput(display, window, KeyPressMask);
    XMapWindow(display, window);

    GC gc = XCreateGC(display, window, 0, NULL);

    BackBuffer back_buffer = {0};
    init_game(&back_buffer, &window_dimensions);

    XImage* image = XCreateImage(
            display, DefaultVisual(display, DefaultScreen(display)),
            DefaultDepth(display, DefaultScreen(display)),
            ZPixmap, 0, NULL, back_buffer.width,
            back_buffer.height, 32, 0);

    b32 should_exit = 0;
    while (!should_exit)
    {
        while (XPending(display))
        {
            XNextEvent(display, &event);
            if (event.type == ClientMessage &&
                    event.xclient.data.l[0] ==
                    XInternAtom(display, "WM_DELETE_WINDOW", False))
            {
                should_exit = 1;
            }
        }

        update_game(&back_buffer);
        image->data = (char*)back_buffer.memory;

        XPutImage(display, window, gc, image, 0, 0, 0, 0,
                back_buffer.width, back_buffer.height);
        XFlush(display);
    }

    XDestroyImage(image);
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
}

