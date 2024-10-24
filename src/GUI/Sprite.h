#pragma once

#include <glm/glm.hpp>

#include "../Graphics/Shader.h"
#include "../Graphics/Texture.h"
#include "../Mesh.h"
#include "GUIObject.h"

class Sprite : public GUIObject {
public:
    Mesh* mesh;
    float* buffer;
    Texture* texture;

    Shader* shader;

    glm::mat4 proj;
    glm::mat4 model;

    float x, y;

    Sprite();
    Sprite(
        float posX, float posY, float width, float height,
        Texture const* texture
    );
    void render();
    void cleanUp();
};
