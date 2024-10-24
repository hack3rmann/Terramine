#pragma once

#include <stdlib.h>

class Mesh;
class Chunk;
class Shader;

class VoxelRenderer {
    float* buffer;
    unsigned long long capacity;

public:
    VoxelRenderer(unsigned long long capacity);
    ~VoxelRenderer();

    Mesh* render(Chunk* chunk, Chunk const** chunks, bool AmbientOcclusion);
    void loadToCache();
};
