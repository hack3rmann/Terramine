#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define BUFFER_OFFSET(i) ((char*)NULL + (i))
#define ASSERT(x) if (!(x)) __debugbreak();
#define glcall(x)                               \
     GLClearError();                            \
     x;                                         \
     ASSERT(GLLogCall(#x, __FILE__, __LINE__));

std::string parseShader(const std::string& name);
unsigned int CompileShader(unsigned int type, const std::string& source);
int CreateShader();
void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

#endif // !RENDERER_H
