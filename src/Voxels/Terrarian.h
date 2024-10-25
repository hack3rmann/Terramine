#pragma once

#include "../graphics/Shader.h"
#include "../graphics/VoxelRenderer.h"
#include "../graphics.hpp"
#include "../Mesh.h"
#include "../Camera.h"
#include "Chunks.h"

class Terrarian {
    friend class TerrarianHandler;

    Mesh** meshes;
    VoxelRenderer renderer;
    bool onceLoad;
    tmine::Texture textureAtlas;
    tmine::Texture normalAtlas;
    vec3 toLightVec;

public:
    Shader* shader;
    Chunks* chunks;
    Terrarian(char const* textureAtlas);
    ~Terrarian();
    void reload();
    void refreshShader();
    void refreshTextures();
    void render(Camera const* cam);
};
