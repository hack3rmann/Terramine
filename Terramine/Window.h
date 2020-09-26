#pragma once
#ifndef WINDOW_H
#define WINDOW_H

struct GLFWwindow;

class Window {
	static GLFWwindow* window;
public:
	static int width;
	static int height;
	static bool viewPortChange;
	static int init(int width, int height, const char* title);
	static void terminate();
	static bool isClosed();
	static void setShouldClose(bool flag);
	static void swapBuffers();
	static void setCursorMode(int mode);

	friend class Events;
};

#endif // !WINDOW_H