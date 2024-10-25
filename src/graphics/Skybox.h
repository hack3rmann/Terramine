#pragma once

#include "../Camera.h"
#include "../Mesh.h"
#include "Shader.h"
#include "../graphics.hpp"

class Skybox {
    friend class SkyboxHandler;

    Mesh* mesh;
    float* buffer;
    unsigned int index;

public:
    Shader* shader;
    tmine::Texture texture;
    Skybox(char const* name);
    void render(Camera const* cam);
};
