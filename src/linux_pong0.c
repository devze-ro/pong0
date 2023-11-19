#include <stdio.h>
#include <string.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>
#include "pong0.h"
#include "pong0.c"

internal long get_nanoseconds_since_epoch() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000L + ts.tv_nsec;
}

internal int get_display_refresh_rate(Display *display) {
    Window root = DefaultRootWindow(display);
    XRRScreenConfiguration *config = XRRGetScreenInfo(display, root);

    Rotation current_rotation;
    int current_rate = XRRConfigCurrentRate(config);

    XRRFreeScreenConfigInfo(config);

    return current_rate;
}

internal void set_key_changed_state(KeyState *key_state, b32 pressed_now)
{
    if (key_state->pressed != pressed_now)
    {
        key_state->pressed = pressed_now;
        key_state->changed = 1;
    }
}

void* linux_init_audio(void* arg)
{
    init_audio();
}

int main()
{
    pthread_t audio_thread;
    int result = pthread_create(&audio_thread, NULL, linux_init_audio, NULL);
    if (result != 0) {
        printf("Audio thread creation failed!");
    }
    
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

    XSelectInput(display, window, KeyPressMask | KeyReleaseMask);
    XMapWindow(display, window);

    GC gc = XCreateGC(display, window, 0, NULL);

    BackBuffer back_buffer = {0};
    init_game(&back_buffer, &window_dimensions);

    XImage* image = XCreateImage(
            display, DefaultVisual(display, DefaultScreen(display)),
            DefaultDepth(display, DefaultScreen(display)),
            ZPixmap, 0, NULL, back_buffer.width,
            back_buffer.height, 32, 0);

    u32 display_refresh_rate = get_display_refresh_rate(display);
    printf("display refresh rate: %d\n", display_refresh_rate);
    long target_seconds_per_frame = (1000000000L / display_refresh_rate);

    InputState input_state = {0};

    b32 should_exit = 0;
    b32 has_game_started = 0;
    b32 is_game_paused = 0;
    b32 is_game_over = 0;
    b32 is_single_player = 1;
    b32 is_sound_muted = 0;
    while (!should_exit)
    {
        long frame_start_time = get_nanoseconds_since_epoch();

        while (XPending(display))
        {
            XNextEvent(display, &event);
            if (event.type == ClientMessage &&
                    event.xclient.data.l[0] ==
                    XInternAtom(display, "WM_DELETE_WINDOW", False))
            {
                should_exit = 1;
            }

            if (event.type == KeyPress || event.type == KeyRelease)
            {
                b32 pressed_now = (event.type == KeyPress);

                KeySym key = XLookupKeysym(&event.xkey, 0);
                switch (key) {
                    case XK_w:
                        if (has_game_started && !is_game_paused &&
                                !is_game_over)
                        {
                            set_key_changed_state(&input_state.w, pressed_now);
                        }
                        break;
                    case XK_s:
                        if (has_game_started && !is_game_paused &&
                                !is_game_over)
                        {
                            set_key_changed_state(&input_state.s, pressed_now);
                        }
                        break;
                    case XK_Up:
                        if (has_game_started && !is_game_paused &&
                                !is_game_over)
                        {
                            set_key_changed_state(&input_state.up, pressed_now);
                        }
                        break;
                    case XK_Down:
                        if (has_game_started && !is_game_paused &&
                                !is_game_over)
                        {
                            set_key_changed_state(&input_state.down,
                                    pressed_now);
                        }
                        break;
                    case XK_p:
                        if (has_game_started && pressed_now)
                        {
                            is_game_paused = is_game_paused ? 0 : 1;
                        }
                        break;
                    case XK_r:
                        if (is_game_over && pressed_now)
                        {
                            is_game_over = 0;
                            init_dynamic_props(&back_buffer);
                        }
                        break;
                    case XK_m:
                        if (pressed_now)
                        {
                            is_sound_muted = is_sound_muted ? 0 : 1;
                        }
                        break;
                    case XK_1:
                        if (pressed_now)
                        {
                            is_single_player = 1;
                            has_game_started = 1;
                        }
                        break;
                    case XK_2:
                        if (pressed_now)
                        {
                            is_single_player = 0;
                            has_game_started = 1;
                        }
                        break;
                    default:
                        break;
                }
            }
        }

        if (has_game_started && !is_game_paused && !is_game_over)
        {
            update_game(&back_buffer, &input_state, 1.0 / 60.0, &is_game_over,
                    is_single_player, is_sound_muted);
        }
        render_game(&back_buffer, &input_state, has_game_started,
                is_game_paused, is_game_over);

        image->data = (char*)back_buffer.memory;

        XPutImage(display, window, gc, image, 0, 0, 0, 0,
                back_buffer.width, back_buffer.height);
        XFlush(display);

        long frame_end_time = get_nanoseconds_since_epoch();
        long frame_duration = frame_end_time - frame_start_time;

        if (frame_duration < target_seconds_per_frame) {
            struct timespec sleep_time;
            sleep_time.tv_sec = 0;
            sleep_time.tv_nsec = target_seconds_per_frame - frame_duration;
            nanosleep(&sleep_time, NULL);
        }

        frame_end_time = get_nanoseconds_since_epoch();
        frame_duration = frame_end_time - frame_start_time;
        double frame_duration_seconds = (double)frame_duration / 1000000000.0;
        printf("Frame Duration: %f seconds\n", frame_duration_seconds);
    }

    XDestroyImage(image);
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
}

