#include "Window.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdio>

#include "types.hpp"

using namespace tmine;

GLFWwindow* Window::glfw_window;
int Window::width = 0;
int Window::height = 0;
bool Window::viewPortChange = true;
bool Window::isHidden = false;

static void window_size_callback(
    [[maybe_unused]] GLFWwindow* window, i32 width, i32 height
) {
    glViewport(0, 0, width, height);
    Window::width = width;
    Window::height = height;
    Window::viewPortChange = true;
}

int Window::init(int width, int height, char const* title) {
    if (!glfwInit()) {
        return -1;
    }

    Window::width = width;
    Window::height = height;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    glfw_window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!glfw_window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(glfw_window);

    auto gl_version = gladLoadGL(glfwGetProcAddress);
    std::printf(
        "GL version %d.%d\n", GLAD_VERSION_MAJOR(gl_version),
        GLAD_VERSION_MINOR(gl_version)
    );

    glViewport(0, 0, width, height);
    glfwSetWindowSizeCallback(glfw_window, window_size_callback);

    return 0;
}

void Window::terminate() {
    glfwTerminate();
}

bool Window::isClosed() { return glfwWindowShouldClose(glfw_window); }

void Window::setShouldClose(bool flag) {
    glfwSetWindowShouldClose(glfw_window, flag);
}

void Window::swapBuffers() { glfwSwapBuffers(glfw_window); }

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::setCursorMode(int mode) {
    glfwSetInputMode(glfw_window, GLFW_CURSOR, mode);
}
