#include "indexBuffer.h"

#include <glad/gl.h>

#include "defines.cpp"

iBuffer::iBuffer(unsigned int const* data, unsigned int count)
    : m_Count(count) {
    glcall(glGenBuffers(1, &m_RendererID));
    glcall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
    glcall(glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data,
        GL_STATIC_DRAW
    ));
}

iBuffer::~iBuffer() { glcall(glDeleteBuffers(1, &m_RendererID)); }

void iBuffer::bind() const {
    glcall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
}

void iBuffer::unbind() const {
    glcall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

inline unsigned int iBuffer::getCount() const { return m_Count; }
