#include "Mesh.h"

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
	glcall(glBindVertexArray(vao));
	glcall(glDrawArrays(primitive, 0, vertices));
	glcall(glBindVertexArray(0));
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