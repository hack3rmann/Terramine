#pragma once

#include <array>

#include "../Camera.h"
#include "../graphics.hpp"
#include "../types.hpp"

class Skybox {
    friend class SkyboxHandler;

public:
    tmine::ShaderProgram shader;
    tmine::Texture texture;
    Skybox(char const* name);
    void render(Camera const* cam);

private:
    tmine::Mesh mesh;
    static auto constexpr VERTEX_ATTRIBUTE_SIZES =
        std::array<tmine::usize, 2>{3, 2};
};
