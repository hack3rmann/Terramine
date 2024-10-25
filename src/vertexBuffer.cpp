#include "vertexBuffer.h"

#include <glad/gl.h>

vBuffer::vBuffer(void const* data, unsigned int size) {
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

vBuffer::~vBuffer() { glDeleteBuffers(1, &m_RendererID); }

void vBuffer::bind() { glBindBuffer(GL_ARRAY_BUFFER, m_RendererID); }

void vBuffer::unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }
