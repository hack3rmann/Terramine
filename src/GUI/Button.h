#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <string>

#include "../graphics.hpp"
#include "../types.hpp"
#include "GUIObject.h"
#include "Text.h"

enum States { Default, onHover, onClick };

class Button : public GUIObject {
    tmine::Texture* textures[3];
    std::function<void()> function;
    static auto constexpr VERTEX_ATTRIBUTE_SIZES =
        std::array<tmine::usize, 3>{2, 2, 4};
    tmine::Mesh mesh;
    States state;
    tmine::ShaderProgram shader;
    float x, y, w, h;
    Text* text;

    glm::mat4 model;

public:
    Button();
    Button(
        float posX, float posY, float width, float height,
        tmine::Texture defTexture, tmine::Texture hoverTexture,
        tmine::Texture clickedTexture, std::string text,
        std::function<void()> function
    );
    static auto get_proj(tmine::f32 aspect_ratio) -> glm::mat4;
    void render(tmine::f32 aspect_ratio);
    void refreshState(glm::uvec2 window_size);
    void cleanUp();
};
