#pragma once

#include <glad/gl.h>
#include <stdlib.h>

class Mesh {
    unsigned int vao;
    unsigned int vbo;
    uint64_t vertices;
    uint64_t vertex_size;

public:
    Mesh(float const* buffer, size_t vertices, int const* attrs);
    ~Mesh();

    void reload(float const* buffer, size_t vertices);
    void draw(unsigned int primitive);
};
