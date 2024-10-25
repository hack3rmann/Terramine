#pragma once

#include <glm/glm.hpp>

#include "../graphics/Shader.h"
#include "../graphics.hpp"
#include "../Mesh.h"
#include "GUIObject.h"

class Sprite : public GUIObject {
public:
    Mesh* mesh;
    float* buffer;
    tmine::Texture texture;

    Shader* shader;

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
