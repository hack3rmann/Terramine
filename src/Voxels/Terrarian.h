#pragma once

#include "../Graphics/FrameBuffer.h"
#include "../Graphics/Shader.h"
#include "../Graphics/Texture.h"
#include "../Graphics/VoxelRenderer.h"
#include "../Mesh.h"
#include "../Player.h"
#include "Chunks.h"

class Terrarian {
    friend class TerrarianHandler;

    Mesh** meshes;
    VoxelRenderer renderer;
    bool onceLoad;
    Texture* textureAtlas;
    Texture* normalAtlas;
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
