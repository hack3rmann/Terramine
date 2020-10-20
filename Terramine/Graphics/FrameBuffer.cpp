#include "FrameBuffer.h"
#include "../defines.cpp"
#include "../Window.h"
#include "../EventHandler.h"

float FrameBuffer::screenQuad[24] = {
//    X      Y       T     S
	/*-1.0f,  0.5f,   0.0f, 0.0f,
	-1.0f,  1.0f,   0.0f, 1.0f,
	-0.5f,  1.0f,   1.0f, 1.0f,
	-1.0f,  0.5f,   0.0f, 0.0f,
	-0.5f,  0.5f,   1.0f, 0.0f,
	-0.5f,  1.0f,   1.0f, 1.0f*/
	-1.0f, -1.0f,   0.0f, 0.0f,
	-1.0f,  1.0f,   0.0f, 1.0f,
	 1.0f,  1.0f,   1.0f, 1.0f,
	-1.0f, -1.0f,   0.0f, 0.0f,
	 1.0f, -1.0f,   1.0f, 0.0f,
	 1.0f,  1.0f,   1.0f, 1.0f
};

FrameBuffer::FrameBuffer(const std::string& vName, const std::string& fName) {
	reload(vName, fName);
}
FrameBuffer::FrameBuffer() {
	reload();
}
void FrameBuffer::bind() {
	glcall(glBindTexture(GL_TEXTURE_2D, 0));
	glcall(glBindFramebuffer(GL_FRAMEBUFFER, fbo));
	glcall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAtt, 0));
	glcall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthAtt, 0));
}
void FrameBuffer::reload(const std::string& vName, const std::string& fName) {
	this->vName = vName;
	this->fName = fName;
	msaa = 4;
	glcall(glGenFramebuffers(1, &fbo));
	glcall(glBindFramebuffer(GL_FRAMEBUFFER, fbo));

	width = Window::width;
	height = Window::height;

	/* Color attachment */
	glcall(glGenTextures(1, &colorAtt));
	glcall(glBindTexture(GL_TEXTURE_2D, colorAtt));
	glcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0));
	glcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	glcall(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	glcall(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	glcall(glBindTexture(GL_TEXTURE_2D, 0));

	/* Depth color attachment */
	glcall(glGenTextures(1, &depthAtt));
	glcall(glBindTexture(GL_TEXTURE_2D, depthAtt));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	glcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL));
	glcall(glBindTexture(GL_TEXTURE_2D, 0));

	/* Attaching */
	glcall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAtt, 0));
	glcall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthAtt, 0));

	/* Screen render vertices */
	glcall(glGenVertexArrays(1, &vao));
	glcall(glGenBuffers(1, &vbo));

	glcall(glBindVertexArray(vao));
	glcall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	glcall(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 6, screenQuad, GL_STATIC_DRAW));
	glcall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (GLvoid*)(0 * sizeof(float))));
	glcall(glEnableVertexAttribArray(0));

	glcall(glBindVertexArray(0));

	screenShader = load_shader(vName, fName);

	/* Attaching color renderbuffer */
	glcall(glGenRenderbuffers(1, &color_rbo));
	glcall(glBindRenderbuffer(GL_RENDERBUFFER, color_rbo));
	glcall(glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa, GL_RGB8, width, height));
	glcall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_rbo));

	/* Attaching depth renderbuffer */
	glcall(glGenRenderbuffers(1, &depth_rbo));
	glcall(glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo));
	glcall(glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa, GL_DEPTH_COMPONENT, width, height));
	glcall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo));

	/* Check OK */
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (status) {
		case GL_FRAMEBUFFER_COMPLETE:
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			CONSOLE_LOG("Frame buffer is unsupported...");
			break;
		default:
			CONSOLE_LOG("Frame buffer is not complete...");
			break;
	}

	/* Unbinding */
	glcall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
	glcall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}
void FrameBuffer::reload() {
	this->vName = vName;
	this->fName = fName;
	msaa = 4;
	glcall(glGenFramebuffers(1, &fbo));
	glcall(glBindFramebuffer(GL_FRAMEBUFFER, fbo));

	width = Window::width;
	height = Window::height;

	/* Color attachment */
	glcall(glGenTextures(1, &colorAtt));
	glcall(glBindTexture(GL_TEXTURE_2D, colorAtt));
	glcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0));
	glcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	glcall(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	glcall(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	glcall(glBindTexture(GL_TEXTURE_2D, 0));

	/* Depth color attachment */
	glcall(glGenTextures(1, &depthAtt));
	glcall(glBindTexture(GL_TEXTURE_2D, depthAtt));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	glcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL));
	glcall(glBindTexture(GL_TEXTURE_2D, 0));

	/* Attaching */
	glcall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAtt, 0));
	glcall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthAtt, 0));

	/* Screen render vertices */
	glcall(glGenVertexArrays(1, &vao));
	glcall(glGenBuffers(1, &vbo));

	glcall(glBindVertexArray(vao));
	glcall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	glcall(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 6, screenQuad, GL_STATIC_DRAW));
	glcall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (GLvoid*)(0 * sizeof(float))));
	glcall(glEnableVertexAttribArray(0));

	glcall(glBindVertexArray(0));

	/* Attaching color renderbuffer */
	glcall(glGenRenderbuffers(1, &color_rbo));
	glcall(glBindRenderbuffer(GL_RENDERBUFFER, color_rbo));
	glcall(glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa, GL_RGB8, width, height));
	glcall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_rbo));

	/* Attaching depth renderbuffer */
	glcall(glGenRenderbuffers(1, &depth_rbo));
	glcall(glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo));
	glcall(glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa, GL_DEPTH_COMPONENT, width, height));
	glcall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo));

	/* Check OK */
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (status) {
		case GL_FRAMEBUFFER_COMPLETE:
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			CONSOLE_LOG("Frame buffer is unsupported...");
			break;
		default:
			CONSOLE_LOG("Frame buffer is not complete...");
			break;
	}

	/* Unbinding */
	glcall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
	glcall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}
void FrameBuffer::unbind() {
	glcall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}
void FrameBuffer::terminate() {
	glcall(glDeleteFramebuffers(1, &fbo));
}
void FrameBuffer::drawColor() {
	screenShader->use();

	/* Vertices */
	glcall(glBindVertexArray(vao));
	glcall(glBindBuffer(GL_ARRAY_BUFFER, vbo));

	/* Binding texture */
	glcall(glBindTexture(GL_TEXTURE_2D, colorAtt));

	/* Draw */
	glcall(glDrawArrays(GL_TRIANGLES, 0, 6));

	/* Unbind vertext array */
	glcall(glBindVertexArray(0));
}
void FrameBuffer::drawDepth() {
	screenShader->use();

	/* Vertices */
	glcall(glBindVertexArray(vao));
	glcall(glBindBuffer(GL_ARRAY_BUFFER, vbo));

	/* Binding texture */
	glcall(glBindTexture(GL_TEXTURE_2D, depthAtt));

	/* Draw */
	glcall(glDrawArrays(GL_TRIANGLES, 0, 6));

	/* Unbind vertext array */
	glcall(glBindVertexArray(0));
}
void FrameBuffer::drawBoth() {
	screenShader->use();

	/* Vertices */
	glcall(glBindVertexArray(vao));
	glcall(glBindBuffer(GL_ARRAY_BUFFER, vbo));

	/* Shader uniforms */
	screenShader->uniform1i("screenDepth", 0);
	screenShader->uniform1i("screenColor", 1);

	/* Binding Textures */
	glcall(glActiveTexture(GL_TEXTURE0));
	glcall(glBindTexture(GL_TEXTURE_2D, depthAtt));
	glcall(glActiveTexture(GL_TEXTURE1));
	glcall(glBindTexture(GL_TEXTURE_2D, colorAtt));

	/* Draw */
	glcall(glDrawArrays(GL_TRIANGLES, 0, 6));

	/* Unbind array */
	glcall(glBindVertexArray(0));
}
int FrameBuffer::check() {
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
		return 1;
	}
	return 0;
}
void FrameBuffer::refreshShader() {
	screenShader = load_shader(vName, fName);
}
void FrameBuffer::bindColorTex() {
	glcall(glBindTexture(GL_TEXTURE_2D, colorAtt));
}
void FrameBuffer::bindDepthTex() {
	glcall(glBindTexture(GL_TEXTURE_2D, depthAtt));
}
