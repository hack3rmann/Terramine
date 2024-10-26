#pragma once

#include <glm/glm.hpp>

#include "../graphics.hpp"
#include "GUIObject.h"

class Sprite : public GUIObject {
public:
    tmine::Mesh mesh;
    static auto constexpr VERTEX_ATTRIBUTE_SIZES =
        std::array<tmine::usize, 3>{2, 2, 4};
    tmine::Texture texture;

    tmine::ShaderProgram shader;

    glm::mat4 proj;
    glm::mat4 model;

    float x, y;

    Sprite();
    Sprite(
        float posX, float posY, float width, float height,
        tmine::Texture texture
    );
    void render();
    void cleanUp();
};
