#pragma once

#include <GL/glew.h>
#include "../defines.cpp"

GLuint createFrameBuffer();
GLuint createDepthBufferAttachment(int width, int height);

class ShadowFrameBuffer {

};