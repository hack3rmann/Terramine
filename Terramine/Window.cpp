#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Window.h"

GLFWwindow* Window::window;

int Window::init(int width, int height, const char* title) {
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

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
	glViewport(NULL, NULL, width, height);

	std::cout << glGetString(GL_VERSION) << std::endl;

	return 0;
}
void Window::terminate() {
	glfwTerminate();
}
bool Window::isClosed() {
	return glfwWindowShouldClose(window);
}
void Window::setShouldClose(bool flag) {
	glfwSetWindowShouldClose(window, flag);
}
void Window::swapBuffers() {
	glfwSwapBuffers(window);
}
