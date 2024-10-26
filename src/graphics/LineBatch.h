#pragma once

#include <array>

#include "../Camera.h"
#include "../graphics.hpp"
#include "../types.hpp"

class Mesh;

class LineBatch {
    friend class LineBatchHandler;

    tmine::Mesh mesh;
    static auto constexpr VERTEX_ATTRIBUTE_SIZES =
        std::array<tmine::usize, 2>{3, 4};

public:
    tmine::ShaderProgram shader;

    LineBatch(tmine::usize capacity);
    ~LineBatch() = default;

    auto line(
        this LineBatch& self, float x1, float y1, float z1, float x2, float y2,
        float z2, float r, float g, float b, float a
    ) -> void;
    auto box(
        this LineBatch& self, float x, float y, float z, float w, float h,
        float d, float r, float g, float b, float a
    ) -> void;
    auto render(this LineBatch& self, Camera const* cam) -> void;
};
