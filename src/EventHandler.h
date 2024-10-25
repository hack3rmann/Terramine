#pragma once

class Events {
public:
    static bool* keys;
    static unsigned int* frames;
    static unsigned int cureent;
    static float dx;
    static float dy;
    static float x;
    static float y;
    static bool cursor_locked;
    static bool cursor_started;

    static int init();
    static void terminate();
    static void pullEvents();
    static bool isPressed(int keycode);
    static bool justPressed(int keycode);
    static bool isClicked(int button);
    static bool justClicked(int button);
    static void toggleCursor();
};
