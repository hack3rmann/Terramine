#include "Window.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdio>

GLFWwindow* Window::window;
int Window::width = 0;
int Window::height = 0;
bool Window::viewPortChange = true;
bool Window::isHidden = false;

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

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    auto gl_version = gladLoadGL(glfwGetProcAddress);
    std::printf(
        "GL version %d.%d\n", GLAD_VERSION_MAJOR(gl_version),
        GLAD_VERSION_MINOR(gl_version)
    );

    glViewport(0, 0, width, height);

    return 0;
}

void Window::terminate() {
    glfwTerminate();
}

bool Window::isClosed() { return glfwWindowShouldClose(window); }

void Window::setShouldClose(bool flag) {
    glfwSetWindowShouldClose(window, flag);
}

void Window::swapBuffers() { glfwSwapBuffers(window); }

void Window::setCursorMode(int mode) {
    glfwSetInputMode(window, GLFW_CURSOR, mode);
}
