#include "Mesh.h"

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

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(float) * vertex_size * vertices, buffer,
        GL_STATIC_DRAW
    );

    /* Attributes */
    int offset = 0;
    for (int i = 0; attrs[i]; i++) {
        int size = attrs[i];
        glVertexAttribPointer(
            i, size, GL_FLOAT, GL_FALSE, vertex_size * sizeof(float),
            (GLvoid*) (offset * sizeof(float))
        );
        glEnableVertexAttribArray(i);
        offset += size;
    }

    glBindVertexArray(0);
}

void Mesh::draw(unsigned int primitive) {
    if (vertices) {
        glBindVertexArray(vao);
        glDrawArrays(primitive, 0, vertices);
        glBindVertexArray(0);
    }
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void Mesh::reload(float const* buffer, size_t vertices) {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(float) * vertex_size * vertices, buffer,
        GL_STATIC_DRAW
    );
    this->vertices = vertices;
}
