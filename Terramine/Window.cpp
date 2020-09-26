#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Window.h"
#include "defines.cpp"

GLFWwindow* Window::window;
int Window::width = 0;
int Window::height = 0;
bool Window::viewPortChange = true;

int Window::init(int width, int height, const char* title) {
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

	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW init error" << std::endl;
		return -1;
	}
	glcall(glViewport(NULL, NULL, width, height));

	std::cout << glGetString(GL_VERSION) << std::endl;

	return 0;
}
void Window::terminate() {
	//delete window;
	glfwTerminate();
}
bool Window::isClosed() {
	glretcall(glfwWindowShouldClose(window));
}
void Window::setShouldClose(bool flag) {
	glcall(glfwSetWindowShouldClose(window, flag));
}
void Window::swapBuffers() {
	glcall(glfwSwapBuffers(window));
}
void Window::setCursorMode(int mode) {
	glfwSetInputMode(window, GLFW_CURSOR, mode);
}
