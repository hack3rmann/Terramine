#pragma once

#include <memory>

#include "graphics.hpp"
#include "controls.hpp"
#include "graphics/FrameBuffer.h"
#include "terrain.hpp"

namespace tmine {

struct SceneParameters {
    glm::vec3 light_direction{glm::normalize(glm::vec3{0.2f, -0.5f, 1.0f})};
};

struct SceneObject {
    virtual ~SceneObject() = default;

    virtual auto render(
        Camera const& camera, SceneParameters const& params,
        glm::uvec2 viewport_size
    ) -> void = 0;
};

class Skybox : public SceneObject {
    friend class SkyboxHandler;

public:
    explicit Skybox(char const* texture_path);
    auto render(
        Camera const& camera, SceneParameters const& params,
        glm::uvec2 viewport_size
    ) -> void override;

private:
    struct Vertex {
        glm::vec3 pos;
        glm::vec2 uv;

        static auto constexpr ATTRIBUTE_SIZES = std::array<usize, 2>{3, 2};
    };

private:
    Mesh<Vertex> mesh;
    ShaderProgram shader;
    Texture texture;
};

class LineBox : public SceneObject {
    friend class LineBatchHandler;

public:
    LineBox();

    auto box(
        this LineBox& self, glm::vec3 pos, glm::vec3 sizes, glm::vec4 color
    ) -> void;

    auto render(
        Camera const& cam, SceneParameters const& params,
        glm::uvec2 viewport_size
    ) -> void override;

private:
    auto line(this LineBox& self, glm::vec3 from, glm::vec3 to, glm::vec4 color)
        -> void;

private:
    struct Vertex {
        glm::vec3 pos;
        glm::vec4 color;

        static auto constexpr ATTRIBUTE_SIZES = std::array<usize, 2>{3, 4};
    };

private:
    ShaderProgram shader;
    Mesh<Vertex> mesh;
};

enum class ChunkState : u8 {
    UpToDate,
    VoxelsUpdated,
    MeshUpdated,
};

class Terrain : public SceneObject {
    friend class TerrarianHandler;

public:
    explicit Terrain(glm::uvec3 sizes);

    auto render(
        Camera const& camera, SceneParameters const& params,
        glm::uvec2 viewport_size
    ) -> void override;

    inline auto get_array(this Terrain const& self) noexcept
        -> ChunkArray const& {
        return self.chunks;
    }

    auto set_voxel(this Terrain& self, glm::uvec3 pos, VoxelId value) -> void;

    auto update(this Terrain& self) -> void;

private:
    auto generate_meshes(this Terrain& self) -> void;

public:
    static char constexpr TEXTURE_ATLAS_PATH[] =
        "assets/images/texture_atlas.png";
    static char constexpr NORMAL_ATLAS_PATH[] =
        "assets/images/normal_atlas.png";
    static char constexpr VERTEX_SHADER_NAME[] = "terrain_vertex.glsl";
    static char constexpr FRAGMENT_SHADER_NAME[] = "terrain_fragment.glsl";
    static char constexpr BLOCK_DATA_PATH[] = "assets/data/blocks.json";
    static char constexpr BLOCK_TEXTURE_DATA_PATH[] =
        "assets/data/block_textures.json";

private:
    ChunkArray chunks;
    std::unique_ptr<Mesh<TerrainRenderer::Vertex>[]> meshes;
    std::vector<usize> chunks_to_update;
    TerrainRenderer renderer;
    ShaderProgram shader;
    Texture texture_atlas;
    Texture normal_atlas;
};

class Scene {
public:
    explicit Scene(glm::uvec2 viewport_size);

    auto render(
        this Scene& self, Camera const& camera, glm::uvec2 viewport_size
    ) -> void;

    auto update_player(this Scene& self, void* player) -> void;

private:
    SceneParameters params{};
    FrameBuffer frame_buffer;
    glm::uvec2 viewport_size;
    std::vector<std::unique_ptr<SceneObject>> objects;
};

}  // namespace tmine
