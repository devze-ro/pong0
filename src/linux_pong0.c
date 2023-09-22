#include <stdio.h>
#include <X11/Xlib.h>

int main() {
    Display* display;
    Window window;
    XEvent event;

    display = XOpenDisplay(NULL);
    if (!display) {
        printf("Cannot open X display\n");
        return 1;
    }

    int screen_num = DefaultScreen(display);
    Window root = RootWindow(display, screen_num);
    int window_width = 640;
    int window_height = 480;

    window = XCreateSimpleWindow(display, root, 0, 0, window_width, window_height, 1,
            BlackPixel(display, screen_num),
            WhitePixel(display, screen_num));

    XStoreName(display, window, "pong0");

    XSelectInput(display, window, KeyPressMask);
    XMapWindow(display, window);

    while (1) {
        XNextEvent(display, &event);
        if (event.type == KeyPress) {
            break;
        }
    }

    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
}

