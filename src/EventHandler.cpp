#include "EventHandler.h"

#include <GLFW/glfw3.h>
#include <string.h>

#include "defines.cpp"

#define _MOUSE_BUTTONS 1024

bool* Events::_keys;
unsigned int* Events::_frames;
unsigned int Events::_current = 0;
float Events::dx = 0.0f;
float Events::dy = 0.0f;
float Events::x = 0.0f;
float Events::y = 0.0f;
bool Events::_cursor_locked = false;
bool Events::_cursor_started = false;

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (Events::_cursor_locked) {
        Events::dx += xpos - Events::x;
        Events::dy += ypos - Events::y;
    } else {
        Events::_cursor_started = true;
    }

    Events::x = xpos;
    Events::y = ypos;
}

void mouse_button_callback(
    GLFWwindow* window, int button, int action, int mode
) {
    if (action == GLFW_PRESS) {
        Events::_keys[_MOUSE_BUTTONS + button] = true;
        Events::_frames[_MOUSE_BUTTONS + button] = Events::_current;
    } else if (action == GLFW_RELEASE) {
        Events::_keys[_MOUSE_BUTTONS + button] = false;
        Events::_frames[_MOUSE_BUTTONS + button] = Events::_current;
    }
}

void key_callback(
    GLFWwindow* window, int key, int scancode, int action, int mode
) {
    if (action == GLFW_PRESS) {
        Events::_keys[key] = true;
        Events::_frames[key] = Events::_current;
    } else if (action == GLFW_RELEASE) {
        Events::_keys[key] = false;
        Events::_frames[key] = Events::_current;
    }
}

void window_size_callback(GLFWwindow* window, int width, int height) {
    glcall(glViewport(0, 0, width, height));
    Window::width = width;
    Window::height = height;
    Window::viewPortChange = true;
}

bool Events::isPressed(int keycode) {
    if (keycode < 0 || keycode >= _MOUSE_BUTTONS) {
        return false;
    }
    return _keys[keycode];
}

bool Events::justPressed(int keycode) {
    if (keycode < 0 || keycode >= _MOUSE_BUTTONS) {
        return false;
    }
    return _keys[keycode] && _frames[keycode] == _current;
}

bool Events::isClicked(int button) { return _keys[_MOUSE_BUTTONS + button]; }

bool Events::justClicked(int button) {
    return _keys[_MOUSE_BUTTONS + button] &&
           _frames[_MOUSE_BUTTONS + button] == _current;
}

int Events::init() {
    GLFWwindow* window = Window::window;
    _keys = new bool[1032];
    _frames = new unsigned int[1032];

    memset(_keys, false, 1032 * sizeof(bool));
    memset(_frames, 0, 1032 * sizeof(unsigned int));

    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);

    return 0;
}

void Events::terminate() {
    delete _keys;
    delete _frames;
}

void Events::pullEvents() {
    _current++;
    dx = 0.0f;
    dy = 0.0f;
    glfwPollEvents();
}

void Events::toggleCursor() {
    _cursor_locked = !_cursor_locked;
    Window::setCursorMode(
        _cursor_locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL
    );
}
