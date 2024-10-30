#pragma once

struct GLFWwindow;

class Window {
public:
    static GLFWwindow* glfw_window;
    static int width;
    static int height;
    static bool viewPortChange;
    static bool isHidden;
    static int init(int width, int height, char const* title);
    static void terminate();
    static bool isClosed();
    static void setShouldClose(bool flag);
    static void swapBuffers();
    static void setCursorMode(int mode);
    static void pollEvents();
};
