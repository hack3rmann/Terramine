#include "indexBuffer.h"

#include <glad/gl.h>

iBuffer::iBuffer(unsigned int const* data, unsigned int count)
    : m_Count(count) {
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data,
        GL_STATIC_DRAW
    );
}

iBuffer::~iBuffer() { glDeleteBuffers(1, &m_RendererID); }

void iBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void iBuffer::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

inline unsigned int iBuffer::getCount() const { return m_Count; }
