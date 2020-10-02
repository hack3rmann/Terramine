#pragma once
#ifndef MESH_H_

#include "defines.cpp"
#include "Graphics/Shader.h"
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

	friend class MeshWithFBO;
};

class MeshWithFBO {
	GLuint vao;
	GLuint vbo;
	GLuint fbo;
	GLuint rbo;
	GLuint QuadVAO;
	GLuint QuadVBO;
	GLuint screenTexture;
	Shader* screenShader;
	unsigned __int64 vertices;
	unsigned __int64 vertex_size;
public:
	MeshWithFBO(const float* buffer, unsigned __int64 vertices, const int* attrs);
	MeshWithFBO(const Mesh& mesh);
	~MeshWithFBO();

	void reload(const float* buffer, unsigned __int64 vertices);
	void draw(GLuint primitive);
	void drawToFBO(GLuint primitive);
	void drawFBOtoScreen(Shader* shader);
	GLuint createTextureAttachment(int width, int height);
	GLuint createDepthTextureAttachment(int width, int height);

	friend class Mesh;
};

#define MESH_H_
#endif // !MESH_H_