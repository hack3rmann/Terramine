#pragma once

#include "../Camera.h"
#include "Shader.h"

class Mesh;

class LineBatch {
    friend class LineBatchHandler;

    Mesh* mesh;
    float* buffer;
    uint64_t index;
    uint64_t capacity;

public:
    Shader* shader;

    LineBatch(uint64_t capacity);
    ~LineBatch();

    void line(
        float x1, float y1, float z1, float x2, float y2, float z2, float r,
        float g, float b, float a
    );
    void box(
        float x, float y, float z, float w, float h, float d, float r, float g,
        float b, float a
    );
    void render(Camera const* cam);
};
