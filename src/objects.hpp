#pragma once

#include <memory>
#include <concepts>

#include "graphics.hpp"
#include "controls.hpp"
#include "terrain.hpp"
#include "panic.hpp"
#include "physics.hpp"

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

    auto clear(this LineBox& self) -> void;

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
        return *self.chunks;
    }

    inline auto borrow_array(this Terrain& self) noexcept
        -> std::shared_ptr<ChunkArray> {
        return self.chunks;
    }

    auto set_voxel(this Terrain& self, glm::uvec3 pos, Voxel value) -> void;

    auto update(this Terrain& self, glm::vec3 camera_pos) -> void;

    inline auto get_data(this Terrain const& self) -> GameBlocksData const& {
        return self.renderer.data;
    }

private:
    auto generate_meshes(this Terrain& self, glm::vec3 camera_pos) -> void;

    auto render_opaque(
        this Terrain& self, Camera const& camera, SceneParameters const& params,
        glm::uvec2 viewport_size
    ) -> void;

    auto render_transparent(
        this Terrain& self, Camera const& camera, SceneParameters const& params,
        glm::uvec2 viewport_size
    ) -> void;

    auto setup_render_resources(
        this Terrain& self, ShaderProgram const& shader, Camera const& camera,
        SceneParameters const& params, glm::uvec2 viewport_size
    ) -> void;

public:
    static char constexpr TEXTURE_ATLAS_PATH[] =
        "assets/images/texture_atlas.png";
    static char constexpr OPAQUE_VERTEX_SHADER_NAME[] =
        "opaque_terrain_vertex.glsl";
    static char constexpr TRANSPARENT_VERTEX_SHADER_NAME[] =
        "transparent_terrain_vertex.glsl";
    static char constexpr FRAGMENT_SHADER_NAME[] = "terrain_fragment.glsl";
    static char constexpr BLOCK_DATA_PATH[] = "assets/data/blocks.json";
    static char constexpr BLOCK_TEXTURE_DATA_PATH[] =
        "assets/data/block_textures.json";

private:
    std::shared_ptr<ChunkArray> chunks;
    std::unique_ptr<Mesh<TerrainRenderer::Vertex>[]> meshes;
    TerrainRenderer::TransparentMesh transparent_mesh{};
    std::vector<usize> chunks_to_update;
    std::vector<usize> chunks_with_transparency;
    TerrainRenderer renderer;
    ShaderProgram opaque_shader;
    ShaderProgram transparent_shader;
    Texture texture_atlas;
    Texture normal_atlas;
};

class TerrainCollider : public Collidable {
public:
    inline explicit TerrainCollider(std::shared_ptr<ChunkArray> chunks)
    : chunks{chunks} {}

    auto get_collidable_bounding_box() const -> Aabb override;

    inline auto get_collider_velocity() const -> glm::vec3 override {
        return glm::vec3{0.0f};
    }

    inline auto set_collider_velocity(glm::vec3) -> void override {}

    inline auto displace_collidable(glm::vec3) -> void override {}

    inline auto is_collidable_dynamic() const -> bool override { return false; }

    inline auto collidable_elasticity() const -> f32 override {
        return ABSOLUTELY_ELASTIC_ELASTICITY;
    }

    auto collide(Collidable const& other) const -> Collision override;

    auto collide_box(this TerrainCollider const& self, BoxCollider const& other)
        -> Collision;

    // TODO: implement `collides` function

private:
    std::shared_ptr<ChunkArray> chunks;
};

class Scene {
public:
    explicit Scene(glm::uvec2 viewport_size);

    auto render(
        this Scene& self, Camera const& camera, glm::uvec2 viewport_size
    ) -> void;

    template <std::derived_from<SceneObject> T>
    auto add_unique(this Scene& self, T value) -> void {
        auto index = self.objects.size();
        self.object_indices.insert({typeid(T).hash_code(), index});
        self.objects.emplace_back(std::make_unique<T>(std::move(value)));
    }

    template <std::derived_from<SceneObject> T>
    auto add(this Scene& self, T value) -> void {
        self.objects.emplace_back(std::make_unique<T>(std::move(value)));
    }

    template <std::derived_from<SceneObject> T>
    auto get(this Scene& self) -> T& {
        if (!self.object_indices.contains(typeid(T).hash_code())) {
            throw Panic(
                "scene contains no objects of type {}", typeid(T).name()
            );
        }

        auto index = self.object_indices.at(typeid(T).hash_code());
        auto& scene_object = self.objects[index];

        return *dynamic_cast<T*>(scene_object.get());
    }

private:
    SceneParameters params{};
    ShaderProgram deferred_shader;
    DeferredRenderer deferred_renderer;
    glm::uvec2 viewport_size;
    std::unordered_map<usize, usize> object_indices{};
    std::vector<std::unique_ptr<SceneObject>> objects{};
};

}  // namespace tmine
