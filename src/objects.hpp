#pragma once

#include <filesystem>

#include "graphics.hpp"
#include "Camera.h"

namespace tmine {

class Skybox {
    friend class SkyboxHandler;

public:
    explicit Skybox(std::filesystem::path texture_path);
    auto render(this Skybox const& self, Camera const& cam) -> void;

public:
    static auto constexpr VERTEX_ATTRIBUTE_SIZES =
        std::array<tmine::usize, 2>{3, 2};

private:
    tmine::Mesh mesh;
    tmine::ShaderProgram shader;
    tmine::Texture texture;
};

}  // namespace tmine
