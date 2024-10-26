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

class LineBox {
    friend class LineBatchHandler;

public:
    LineBox();

    auto box(
        this LineBox& self, glm::vec3 pos, glm::vec3 sizes, glm::vec4 color
    ) -> void;

    auto render(this LineBox const& self, Camera const& cam) -> void;

private:
    auto line(
        this LineBox& self, glm::vec3 from, glm::vec3 to, glm::vec4 color
    ) -> void;

private:
    tmine::ShaderProgram shader;
    tmine::Mesh mesh;

    static auto constexpr VERTEX_ATTRIBUTE_SIZES =
        std::array<tmine::usize, 2>{3, 4};
};

}  // namespace tmine
