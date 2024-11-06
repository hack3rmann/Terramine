#pragma once

#include <glm/glm.hpp>

#include "../graphics.hpp"
#include "GUIObject.h"

class OldSprite : public GUIObject {
public:
    tmine::Mesh<GUIObject::Vertex> mesh;
    tmine::Texture texture;

    tmine::ShaderProgram shader;

    glm::mat4 model;

    float x, y;

    OldSprite();
    OldSprite(float posX, float posY, tmine::f32 size, tmine::Texture texture);
    static auto get_proj(tmine::f32 aspect_ratio) -> glm::mat4;
    void render(tmine::f32 aspect_ratio);
};
