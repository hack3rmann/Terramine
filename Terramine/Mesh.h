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

#define MESH_H_
#endif // !MESH_H_