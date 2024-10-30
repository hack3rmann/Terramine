#pragma once

#include "../graphics.hpp"

class Chunk;

class VoxelRenderer {
    static auto constexpr VERTEX_ATTRIBUTE_SIZES =
        std::array<tmine::usize, 5>{3, 3, 2, 1, 3};

public:
    VoxelRenderer() = default;
    ~VoxelRenderer() = default;

    tmine::Mesh render(
        Chunk* chunk, Chunk const** chunks, bool AmbientOcclusion
    );
};
