#include "Mesh.h"
#include "Window.h"
#include "Graphics/Texture.h"

Mesh::Mesh(const float* buffer, size_t vertices, const int* attrs) : vertices(vertices) {
	vertex_size = 0;
	for (int i = 0; attrs[i]; i++) {
		vertex_size += attrs[i];
	}

	glcall(glGenVertexArrays(1, &vao));
	glcall(glGenBuffers(1, &vbo));

	glcall(glBindVertexArray(vao));
	glcall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	glcall(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_size * vertices, buffer, GL_STATIC_DRAW));

	/* Attributes */
	int offset = 0;
	for (int i = 0; attrs[i]; i++) {
		int size = attrs[i];
		glcall(glVertexAttribPointer(i, size, GL_FLOAT, GL_FALSE, vertex_size * sizeof(float), (GLvoid*)(offset * sizeof(float))));
		glcall(glEnableVertexAttribArray(i));
		offset += size;
	}

	glcall(glBindVertexArray(0));
}
void Mesh::draw(unsigned int primitive) {
	if (vertices) {
		glcall(glBindVertexArray(vao));
		glcall(glDrawArrays(primitive, 0, vertices));
		glcall(glBindVertexArray(0));
	}
}
Mesh::~Mesh() {
	glcall(glDeleteVertexArrays(1, &vao));
	glcall(glDeleteBuffers(1, &vbo));
}
void Mesh::reload(const float* buffer, size_t vertices) {
	glcall(glBindVertexArray(vao));
	glcall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	glcall(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_size * vertices, buffer, GL_STATIC_DRAW));
	this->vertices = vertices;
}

MeshWithFBO::MeshWithFBO(const float* buffer, size_t vertices, const int* attrs) : vertices(vertices) {
	vertex_size = 0;
	for (int i = 0; attrs[i]; i++) {
		vertex_size += attrs[i];
	}

	glcall(glGenVertexArrays(1, &vao));
	glcall(glGenBuffers(1, &vbo));
	glcall(glGenBuffers(1, &fbo));
	glcall(glGenBuffers(1, &dbo));

	glcall(glBindVertexArray(vao));
	glcall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	glcall(glBindBuffer(GL_FRAMEBUFFER, fbo));
	glcall(glBindBuffer(GL_RENDERBUFFER, dbo));
	glcall(glDrawBuffer(GL_COLOR_ATTACHMENT0));
	glcall(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_size * vertices, buffer, GL_STATIC_DRAW));
	glcall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Window::width, Window::height));
	glcall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dbo));

	/* Attributes */
	int offset = 0;
	for (int i = 0; attrs[i]; i++) {
		int size = attrs[i];
		glcall(glVertexAttribPointer(i, size, GL_FLOAT, GL_FALSE, vertex_size * sizeof(float), (GLvoid*)(offset * sizeof(float))));
		glcall(glEnableVertexAttribArray(i));
		offset += size;
	}

	glcall(glBindVertexArray(0));
}
void MeshWithFBO::draw(GLuint primitive) {
	if (vertices) {
		glcall(glBindVertexArray(vao));
		glcall(glDrawArrays(primitive, 0, vertices));
		glcall(glBindVertexArray(0));
	}
}
void MeshWithFBO::drawToFBO(GLuint primitive) {
	if (vertices) {
		Texture::unbind();
		glcall(glBindBuffer(GL_FRAMEBUFFER, fbo));
		glcall(glViewport(0, 0, Window::width, Window::height));
		glcall(glBindVertexArray(vao));
		glcall(glDrawArrays(primitive, 0, vertices));
		glcall(glBindVertexArray(0));
	}
}
MeshWithFBO::~MeshWithFBO() {
	glcall(glDeleteVertexArrays(1, &vao));
	glcall(glDeleteBuffers(1, &vbo));
	glcall(glDeleteBuffers(1, &fbo));
}
void MeshWithFBO::reload(const float* buffer, size_t vertices) {
	glcall(glBindVertexArray(vao));
	glcall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	glcall(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_size * vertices, buffer, GL_STATIC_DRAW));
	glcall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Window::width, Window::height));
	this->vertices = vertices;
}
GLuint MeshWithFBO::createTextureAttachment(int width, int height) {
	GLuint texture;
	glcall(glGenTextures(1, &texture));
	glcall(glBindTexture(GL_TEXTURE_2D, texture));
	glcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)0));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	glcall(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0));
	glcall(glBindTexture(GL_TEXTURE_2D, 0));
	return texture;
}
GLuint MeshWithFBO::createDepthTextureAttachment(int width, int height) {
	GLuint texture;
	glcall(glGenTextures(1, &texture));
	glcall(glBindTexture(GL_TEXTURE_2D, texture));
	glcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, (void*)0));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	glcall(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0));
	glcall(glBindTexture(GL_TEXTURE_2D, 0));
	return texture;
}