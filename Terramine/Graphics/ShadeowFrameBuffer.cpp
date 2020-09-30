#include "ShadeowFrameBuffer.h"

GLuint createFrameBuffer() {
	GLuint fbo;
	glcall(glGenBuffers(1, &fbo));
	glcall(glBindFramebuffer(GL_FRAMEBUFFER, fbo));
	glcall(glDrawBuffer(GL_NONE));
	return fbo;
}
GLuint createDepthBufferAttachment(int width, int height) {
	GLuint texture;
	glcall(glGenTextures(1, &texture));
	glcall(glBindTexture(GL_TEXTURE_2D, texture));
	glcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, (void*)0));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	glcall(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0));
	return texture;
}