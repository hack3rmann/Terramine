#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <string>

#include "../Graphics/Texture.h"
#include "../Mesh.h"
#include "GUIObject.h"
#include "Text.h"

enum States { Default, onHover, onClick };

class Button : public GUIObject {
    Texture* textures[3];
    std::function<void()> function;
    Mesh* mesh;
    float* buffer;
    States state;
    Shader* shader;
    float x, y, w, h;
    Text* text;

    glm::mat4 proj;
    glm::mat4 model;

public:
    Button();
    Button(
        float posX, float posY, float width, float height,
        Texture const* defTexture, Texture const* hoverTexture,
        Texture const* clickedTexture, std::string text,
        std::function<void()> function
    );
    void render();
    void refreshState();
    void cleanUp();
};
