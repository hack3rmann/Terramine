#pragma once
#ifndef MESH_H_

#include "defines.cpp"
#include <stdlib.h>
#include <GL/glew.h>

class Mesh {
	unsigned int vao;
	unsigned int vbo;
	unsigned __int64 vertices;
	unsigned __int64 vertex_size;
public:
	Mesh(const float* buffer, size_t vertices, const int* attrs);
	~Mesh();

	void reload(const float* buffer, size_t vertices);
	void draw(unsigned int primitive);
};

class MeshWithFBO {
	GLuint vao;
	GLuint vbo;
	GLuint fbo;
	GLuint dbo;
	unsigned __int64 vertices;
	unsigned __int64 vertex_size;
public:
	MeshWithFBO(const float* buffer, size_t vertices, const int* attrs);
	~MeshWithFBO();

	void reload(const float* buffer, size_t vertices);
	void draw(GLuint primitive);
	void drawToFBO(GLuint primitive);
	GLuint createTextureAttachment(int width, int height);
	GLuint createDepthTextureAttachment(int width, int height);
};

#define MESH_H_
#endif // !MESH_H_