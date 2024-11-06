#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <string>

#include "../graphics.hpp"
#include "../types.hpp"
#include "../gui.hpp"
#include "GUIObject.h"

enum States { Default, OnHover, OnClick };

class OldButton : public GUIObject {
    tmine::Texture textures[3];
    std::function<void()> function;
    tmine::Mesh<GUIObject::Vertex> mesh;
    States state;
    tmine::ShaderProgram shader;
    float x, y, w, h;
    tmine::Text text;

    glm::mat4 model;

public:
    OldButton();
    OldButton(
        tmine::Font const& font, tmine::Texture glyph_texture, float posX,
        float posY, tmine::f32 size, tmine::Texture defTexture,
        tmine::Texture hoverTexture, tmine::Texture clickedTexture,
        std::string text, std::function<void()> function
    );
    static auto get_proj(tmine::f32 aspect_ratio) -> glm::mat4;
    void render(glm::uvec2 viewport_size);
    void refreshState(glm::uvec2 window_size);
};
