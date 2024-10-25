#pragma once

#include <cstdint>

class Mesh;
class Chunk;

class VoxelRenderer {
    float* buffer;
    unsigned long long capacity;

public:
    VoxelRenderer(std::uint64_t capacity);
    ~VoxelRenderer();

    Mesh* render(Chunk* chunk, Chunk const** chunks, bool AmbientOcclusion);
    void loadToCache();
};
