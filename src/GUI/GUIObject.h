#pragma once

#include <array>
#include <glm/glm.hpp>

#include "../types.hpp"

class GUIObject {
public:
    float posX, posY, width, height;
    GUIObject(float posX, float posY, float width, float height);
    GUIObject();

    struct Vertex {
        glm::vec2 pos;
        glm::vec2 uv;
        glm::vec4 color;

        static auto constexpr ATTRIBUTE_SIZES =
            std::array<tmine::usize, 3>{2, 2, 4};
    };
};
