#pragma once

#include "../Camera.h"
#include "../Mesh.h"
#include "Shader.h"
#include "Texture.h"

class Skybox {
    friend class SkyboxHandler;

    Mesh* mesh;
    float* buffer;
    unsigned int index;

public:
    Shader* shader;
    Texture* texture;
    Skybox(char const* name);
    void render(Camera const* cam);
};
