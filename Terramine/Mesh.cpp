#include "Mesh.h"
#include "Window.h"
#include "Graphics/Texture.h"

float screen[] = {
//    X      Y       T     S
	-1.0f, -1.0f,   0.0f, 0.0f,
	-1.0f,  1.0f,   0.0f, 1.0f,
	 1.0f,  1.0f,   1.0f, 1.0f,
	-1.0f, -1.0f,   0.0f, 0.0f,
	 1.0f, -1.0f,   1.0f, 0.0f,
	 1.0f,  1.0f,   1.0f, 1.0f
};

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

MeshWithFBO::MeshWithFBO(const float* buffer, unsigned __int64 vertices, const int* attrs) : vertices(vertices) {
	vertex_size = 0;
	for (int i = 0; attrs[i]; i++)
		vertex_size += attrs[i];

	glcall(glGenVertexArrays(1, &vao));
	glcall(glGenVertexArrays(1, &QuadVAO));
	glcall(glGenBuffers(1, &vbo));
	glcall(glGenBuffers(1, &QuadVBO));
	glcall(glGenFramebuffers(1, &fbo));
	glcall(glGenRenderbuffers(1, &rbo));

	glcall(glBindVertexArray(QuadVAO));
	glcall(glBindBuffer(GL_ARRAY_BUFFER, QuadVBO));
	glcall(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, screen, GL_STATIC_DRAW));

	screenTexture = createTextureAttachment(Window::width, Window::height);

	glcall(glBindVertexArray(vao));
	glcall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	glcall(glBindFramebuffer(GL_FRAMEBUFFER, fbo));
	glcall(glBindRenderbuffer(GL_RENDERBUFFER, rbo));
	glcall(glDrawBuffer(GL_COLOR_ATTACHMENT0));
	glcall(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_size * vertices, buffer, GL_STATIC_DRAW));
	glcall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Window::width, Window::height));
	glcall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo));

	/* Attributes */
	int offset = 0;
	for (int i = 0; attrs[i]; i++) {
		int size = attrs[i];
		glcall(glVertexAttribPointer(i, size, GL_FLOAT, GL_FALSE, vertex_size * sizeof(float), (GLvoid*)(offset * sizeof(float))));
		glcall(glEnableVertexAttribArray(i));
		offset += size;
	}

	glcall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
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
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		draw(primitive);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
void MeshWithFBO::drawFBOtoScreen(Shader* shader) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(QuadVAO);
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	shader->use();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
MeshWithFBO::~MeshWithFBO() {
	glcall(glDeleteVertexArrays(1, &vao));
	glcall(glDeleteBuffers(1, &vbo));
	glcall(glDeleteFramebuffers(1, &fbo));
}
void MeshWithFBO::reload(const float* buffer, unsigned __int64 vertices) {
	glcall(glBindVertexArray(vao));
	glcall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	glcall(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_size * vertices, buffer, GL_STATIC_DRAW));
	glcall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Window::width, Window::height));
	this->vertices = vertices;
}
GLuint MeshWithFBO::createTextureAttachment(int width, int height) {
	/*GLuint texture;
	glcall(glGenTextures(1, &texture));
	glcall(glBindTexture(GL_TEXTURE_2D, texture));
	glcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)0));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	glcall(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0));
	glcall(glBindTexture(GL_TEXTURE_2D, 0));
	return texture;*/

	GLuint texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// присоедиение текстуры к объекту текущего кадрового буфера
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	return texColorBuffer;
}
GLuint MeshWithFBO::createDepthTextureAttachment(int width, int height) {
	GLuint texture;
	glcall(glGenTextures(1, &texture));
	glcall(glBindTexture(GL_TEXTURE_2D, texture));
	glcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, (void*)0));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	glcall(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0));
	glcall(glBindTexture(GL_TEXTURE_2D, 0));
	return texture;
}
MeshWithFBO::MeshWithFBO(const Mesh& mesh) {
	vao = mesh.vao;
	vbo = mesh.vbo;
	vertices = mesh.vertices;
	vertex_size = mesh.vertex_size;

	glcall(glGenVertexArrays(1, &QuadVAO));
	glcall(glGenBuffers(1, &QuadVBO));

	glcall(glGenFramebuffers(1, &fbo));
	glcall(glGenRenderbuffers(1, &rbo));
	glcall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Window::width, Window::height));
	glcall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo));

	glcall(glBindVertexArray(QuadVAO));
	glcall(glBindBuffer(GL_ARRAY_BUFFER, QuadVBO));
	glcall(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, screen, GL_STATIC_DRAW));
}