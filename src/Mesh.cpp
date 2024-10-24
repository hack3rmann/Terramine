#include "Mesh.h"

#include "Graphics/Texture.h"
#include "Window.h"

float screen[] = {
    //    X      Y       T     S
    -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f,  0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f
};

Mesh::Mesh(float const* buffer, size_t vertices, int const* attrs)
    : vertices(vertices) {
    vertex_size = 0;
    for (int i = 0; attrs[i]; i++) {
        vertex_size += attrs[i];
    }

    glcall(glGenVertexArrays(1, &vao));
    glcall(glGenBuffers(1, &vbo));

    glcall(glBindVertexArray(vao));
    glcall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    glcall(glBufferData(
        GL_ARRAY_BUFFER, sizeof(float) * vertex_size * vertices, buffer,
        GL_STATIC_DRAW
    ));

    /* Attributes */
    int offset = 0;
    for (int i = 0; attrs[i]; i++) {
        int size = attrs[i];
        glcall(glVertexAttribPointer(
            i, size, GL_FLOAT, GL_FALSE, vertex_size * sizeof(float),
            (GLvoid*) (offset * sizeof(float))
        ));
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

void Mesh::reload(float const* buffer, size_t vertices) {
    glcall(glBindVertexArray(vao));
    glcall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    glcall(glBufferData(
        GL_ARRAY_BUFFER, sizeof(float) * vertex_size * vertices, buffer,
        GL_STATIC_DRAW
    ));
    this->vertices = vertices;
}
