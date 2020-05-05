#include "vertexBuffer.h"

vBuffer::vBuffer(const void* data, unsigned int size) {
	glcall(glGenBuffers(1, &m_RendererID));
	glcall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
	glcall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}
vBuffer::~vBuffer() {
	glcall(glDeleteBuffers(1, &m_RendererID));
}
void vBuffer::bind() {
	glcall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}
void vBuffer::unbind() {
	glcall(glBindBuffer(GL_ARRAY_BUFFER, NULL));
}
