#include "EventHandler.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <string.h>

#include "Window.h"

#define _MOUSE_BUTTONS 1024

bool* Events::keys;
unsigned int* Events::frames;
unsigned int Events::cureent = 0;
float Events::dx = 0.0f;
float Events::dy = 0.0f;
float Events::x = 0.0f;
float Events::y = 0.0f;
bool Events::cursor_locked = false;
bool Events::cursor_started = false;

void cursor_position_callback(
    [[maybe_unused]] GLFWwindow* window, double xpos, double ypos
) {
    if (Events::cursor_locked) {
        Events::dx += xpos - Events::x;
        Events::dy += ypos - Events::y;
    } else {
        Events::cursor_started = true;
    }

    Events::x = xpos;
    Events::y = ypos;
}

void mouse_button_callback(
    [[maybe_unused]] GLFWwindow* window, int button, int action,
    [[maybe_unused]] int mode
) {
    if (action == GLFW_PRESS) {
        Events::keys[_MOUSE_BUTTONS + button] = true;
        Events::frames[_MOUSE_BUTTONS + button] = Events::cureent;
    } else if (action == GLFW_RELEASE) {
        Events::keys[_MOUSE_BUTTONS + button] = false;
        Events::frames[_MOUSE_BUTTONS + button] = Events::cureent;
    }
}

void key_callback(
    [[maybe_unused]] GLFWwindow* window, int key, [[maybe_unused]] int scancode,
    int action, [[maybe_unused]] int mode
) {
    if (action == GLFW_PRESS) {
        Events::keys[key] = true;
        Events::frames[key] = Events::cureent;
    } else if (action == GLFW_RELEASE) {
        Events::keys[key] = false;
        Events::frames[key] = Events::cureent;
    }
}

void window_size_callback(
    [[maybe_unused]] GLFWwindow* window, int width, int height
) {
    glViewport(0, 0, width, height);
    Window::width = width;
    Window::height = height;
    Window::viewPortChange = true;
}

bool Events::isPressed(int keycode) {
    if (keycode < 0 || keycode >= _MOUSE_BUTTONS) {
        return false;
    }
    return keys[keycode];
}

bool Events::justPressed(int keycode) {
    if (keycode < 0 || keycode >= _MOUSE_BUTTONS) {
        return false;
    }
    return keys[keycode] && frames[keycode] == cureent;
}

bool Events::isClicked(int button) { return keys[_MOUSE_BUTTONS + button]; }

bool Events::justClicked(int button) {
    return keys[_MOUSE_BUTTONS + button] &&
           frames[_MOUSE_BUTTONS + button] == cureent;
}

int Events::init() {
    GLFWwindow* window = Window::window;
    keys = new bool[1032];
    frames = new unsigned int[1032];

    memset(keys, false, 1032 * sizeof(*keys));
    memset(frames, 0, 1032 * sizeof(*frames));

    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);

    return 0;
}

void Events::terminate() {
    delete[] keys;
    delete[] frames;
}

void Events::pullEvents() {
    cureent++;
    dx = 0.0f;
    dy = 0.0f;
    glfwPollEvents();
}

void Events::toggleCursor() {
    cursor_locked = !cursor_locked;
    Window::setCursorMode(
        cursor_locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL
    );
}
