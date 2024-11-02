#pragma once

#include <memory>

#include "graphics.hpp"
#include "Camera.h"
#include "terrain.hpp"

namespace tmine {

class Skybox {
    friend class SkyboxHandler;

public:
    explicit Skybox(char const* texture_path);
    auto render(
        this Skybox const& self, Camera const& cam, glm::uvec2 window_size
    ) -> void;

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

    auto render(this LineBox const& self, Camera const& cam, f32 aspect_ratio) -> void;

private:
    auto line(this LineBox& self, glm::vec3 from, glm::vec3 to, glm::vec4 color)
        -> void;

private:
    tmine::ShaderProgram shader;
    tmine::Mesh mesh;

    static auto constexpr VERTEX_ATTRIBUTE_SIZES =
        std::array<tmine::usize, 2>{3, 4};
};

class Terrain {
    friend class TerrarianHandler;

public:
    explicit Terrain(glm::uvec3 sizes);

    auto render(
        this Terrain const& self, Camera const& cam, glm::uvec3 light_direction,
        glm::uvec2 window_size
    ) -> void;

    inline auto get_array(this Terrain const& self) noexcept
        -> ChunkArray const& {
        return self.chunks;
    }

    auto set_voxel(this Terrain& self, glm::uvec3 pos, VoxelId value) -> void;

    auto update(this Terrain& self) -> void;

private:
    auto generate_meshes(this Terrain& self) -> void;

public:
    static char constexpr TEXTURE_ATLAS_PATH[] = "assets/texture_atlas.png";
    static char constexpr NORMAL_ATLAS_PATH[] = "assets/normal_atlas.png";
    static char constexpr VERTEX_SHADER_NAME[] = "vertexShader.glsl";
    static char constexpr FRAGMENT_SHADER_NAME[] = "fragmentShader.glsl";
    static char constexpr BLOCK_DATA_PATH[] = "assets/blocks.json";
    static char constexpr BLOCK_TEXTURE_DATA_PATH[] =
        "assets/block_textures.json";

private:
    ChunkArray chunks;
    std::unique_ptr<Mesh[]> meshes;
    std::unique_ptr<bool[]> is_modified;
    TerrainRenderer renderer;
    ShaderProgram shader;
    Texture texture_atlas;
    Texture normal_atlas;
};

}  // namespace tmine
