#include <ranges>
#include <cstdlib>

#include "../objects.hpp"
#include "../loaders.hpp"
#include "../window.hpp"

namespace tmine {

namespace rg = std::ranges;
namespace vs = std::ranges::views;

// Idiomatic dedup algorithm taken from
// <https://www.geeksforgeeks.org/remove-duplicates-from-vector-in-cpp/>
template <class T>
static auto dedup_vector(std::vector<T>* vec) -> void {
    rg::sort(*vec);
    auto it = std::unique(vec->begin(), vec->end());
    vec->erase(it, vec->end());
}

Terrain::Terrain(glm::uvec3 sizes)
: chunks{sizes}
, meshes{std::make_unique<Mesh<TerrainRenderer::Vertex>[]>(
      sizes.x * sizes.y * sizes.z
  )}
, chunks_to_update(sizes.x * sizes.y * sizes.z)
, renderer{load_game_blocks_data(
      Terrain::BLOCK_DATA_PATH, Terrain::BLOCK_TEXTURE_DATA_PATH
  )}
, opaque_shader{load_shader(
      Terrain::OPAQUE_VERTEX_SHADER_NAME, Terrain::FRAGMENT_SHADER_NAME
  )}
, transparent_shader{load_shader(
      Terrain::TRANSPARENT_VERTEX_SHADER_NAME, Terrain::FRAGMENT_SHADER_NAME
  )}
, texture_atlas{Texture::from_image(
      load_png(Terrain::TEXTURE_ATLAS_PATH), TextureLoad::DEFAULT
  )}
, normal_atlas{Texture::from_image(
      load_png(Terrain::NORMAL_ATLAS_PATH), TextureLoad::DEFAULT
  )} {
    auto const n_meshes = sizes.x * sizes.y * sizes.z;

    for (usize i = 0; i < n_meshes; ++i) {
        this->chunks_to_update[i] = i;
    }

    this->generate_meshes(glm::vec3{0.0f});
}

static auto sort_transparent_triangles(
    RefMut<TerrainRenderer::TransparentMesh> mesh, glm::vec3 camera_pos
) -> void {
    auto vertices = mesh->get_buffer().lock();

    if (vertices.size() % 3 != 0) {
        throw Panic("transparent mesh size should be divisible by 3");
    }

    struct Triangle {
        std::array<TerrainRenderer::TransparentVertex, 3> vertices;

        auto center(this Triangle const& self) -> glm::vec3 {
            return (self.vertices[0].pos + self.vertices[1].pos +
                    self.vertices[2].pos) /
                   3.0f;
        }
    };

    auto triangles = std::span<Triangle>{
        reinterpret_cast<Triangle*>(vertices.begin()),
        reinterpret_cast<Triangle*>(vertices.end())
    };

    auto manhattan_comparator =
        [camera_pos](auto const& left, auto const& right) {
            auto pos = 3.0f * camera_pos;

            auto left_center = left.vertices[0].pos + left.vertices[1].pos +
                               left.vertices[2].pos;
            auto left_distance = glm::abs(pos.x - left_center.x) +
                                 glm::abs(pos.y - left_center.y) +
                                 glm::abs(pos.z - left_center.z);

            auto right_center = right.vertices[0].pos + right.vertices[1].pos +
                                right.vertices[2].pos;
            auto right_distance = glm::abs(pos.x - right_center.x) +
                                  glm::abs(pos.y - right_center.y) +
                                  glm::abs(pos.z - right_center.z);

            return left_distance > right_distance;
        };

    rg::stable_sort(triangles, manhattan_comparator);
}

auto Terrain::generate_meshes(this Terrain& self, glm::vec3 camera_pos)
    -> void {
    // Remove duplicates to prevent data race
    dedup_vector(&self.chunks_to_update);

    if (!self.chunks_to_update.empty()) {
        self.transparent_mesh.get_buffer().clear();
    }

#pragma omp parallel for
    for (auto i : self.chunks_to_update) {
        auto const pos = self.chunks.index_to_pos(i);

        // Do not reload mesh buffer on multithread
        self.renderer.render(
            self.chunks, pos, &self.meshes[i], &self.transparent_mesh,
            TerrainRenderUploadMesh::Skip
        );
    }

    sort_transparent_triangles(&self.transparent_mesh, camera_pos);
    self.transparent_mesh.reload_buffer();

    // Reload buffers on main thread
    for (auto i : self.chunks_to_update) {
        self.meshes[i].reload_buffer();
    }

    self.chunks_to_update.clear();
}

auto Terrain::update(this Terrain& self, glm::vec3 camera_pos) -> void {
    self.generate_meshes(camera_pos);
}

auto Terrain::render(
    Camera const& cam, SceneParameters const& params, glm::uvec2 viewport_size
) -> void {
    this->update(cam.get_pos());

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    this->render_opaque(cam, params, viewport_size);

    glDisable(GL_CULL_FACE);

    this->render_transparent(cam, params, viewport_size);

    glDisable(GL_DEPTH_TEST);
}

auto Terrain::render_transparent(
    this Terrain& self, Camera const& cam, SceneParameters const& params,
    glm::uvec2 viewport_size
) -> void {
    self.transparent_shader.bind();
    self.transparent_shader.uniform_mat4(
        "proj", cam.get_projection(Window::aspect_ratio_of(viewport_size))
    );
    self.transparent_shader.uniform_mat4("view", cam.get_view());
    self.transparent_shader.uniform_vec2(
        "resolution", glm::vec2{viewport_size}
    );
    self.transparent_shader.uniform_vec3("toLightVec", -params.light_direction);
    self.transparent_shader.uniform_vec3(
        "lightColor", glm::vec3(0.96f, 0.24f, 0.0f)
    );
    self.transparent_shader.uniform_int("u_Texture0", 0);
    self.transparent_shader.uniform_int("u_Texture1", 1);

    self.texture_atlas.bind(0);
    self.normal_atlas.bind(1);

    self.transparent_mesh.draw();
}

auto Terrain::render_opaque(
    this Terrain& self, Camera const& cam, SceneParameters const& params,
    glm::uvec2 viewport_size
) -> void {
    self.opaque_shader.bind();
    self.opaque_shader.uniform_mat4(
        "proj", cam.get_projection(Window::aspect_ratio_of(viewport_size))
    );
    self.opaque_shader.uniform_mat4("view", cam.get_view());
    self.opaque_shader.uniform_vec2("resolution", glm::vec2{viewport_size});
    self.opaque_shader.uniform_vec3("toLightVec", -params.light_direction);
    self.opaque_shader.uniform_vec3(
        "lightColor", glm::vec3(0.96f, 0.24f, 0.0f)
    );
    self.opaque_shader.uniform_int("u_Texture0", 0);
    self.opaque_shader.uniform_int("u_Texture1", 1);

    self.texture_atlas.bind(0);
    self.normal_atlas.bind(1);

    auto const n_chunks = self.chunks.chunk_count();
    auto model = glm::mat4{1.0f};
    auto meshes = std::span{self.meshes.get(), self.meshes.get() + n_chunks};

    for (auto [chunk, mesh] : vs::zip(self.chunks.as_span(), meshes)) {
        auto const pos = chunk.get_pos();
        auto const offset =
            glm::vec3{pos} * glm::vec3{Chunk::SIZES} + glm::vec3{0.5f};

        model = glm::translate(glm::mat4{1.0f}, offset);

        self.opaque_shader.uniform_mat4("model", model);
        mesh.draw();
    }
}

auto Terrain::set_voxel(this Terrain& self, glm::uvec3 pos, VoxelId value)
    -> void {
    auto const chunk_pos = pos / Chunk::SIZES;
    auto const local_pos = pos % Chunk::SIZES;

    if (!self.chunks.is_in_bounds(chunk_pos) || !Chunk::is_in_bounds(local_pos))
    {
        return;
    }

    auto const sizes = self.chunks.get_sizes();

    self.chunks_to_update.push_back(self.chunks.index_of(chunk_pos));

    if (0 == local_pos.x && 0 != chunk_pos.x) {
        self.chunks_to_update.push_back(self.chunks.index_of(
            glm::uvec3(chunk_pos.x - 1, chunk_pos.y, chunk_pos.z)
        ));
    }

    if (Chunk::WIDTH == local_pos.x + 1 && sizes.x != chunk_pos.x + 1) {
        self.chunks_to_update.push_back(self.chunks.index_of(
            glm::uvec3(chunk_pos.x + 1, chunk_pos.y, chunk_pos.z)
        ));
    }

    if (0 == local_pos.y && 0 != chunk_pos.y) {
        self.chunks_to_update.push_back(self.chunks.index_of(
            glm::uvec3(chunk_pos.x, chunk_pos.y - 1, chunk_pos.z)
        ));
    }

    if (Chunk::HEIGHT == local_pos.y + 1 && sizes.y != chunk_pos.y + 1) {
        self.chunks_to_update.push_back(self.chunks.index_of(
            glm::uvec3(chunk_pos.x, chunk_pos.y + 1, chunk_pos.z)
        ));
    }

    if (0 == local_pos.z && 0 != chunk_pos.z) {
        self.chunks_to_update.push_back(self.chunks.index_of(
            glm::uvec3(chunk_pos.x, chunk_pos.y, chunk_pos.z - 1)
        ));
    }

    if (Chunk::DEPTH == local_pos.z + 1 && sizes.z != chunk_pos.z + 1) {
        self.chunks_to_update.push_back(self.chunks.index_of(
            glm::uvec3(chunk_pos.x, chunk_pos.y, chunk_pos.z + 1)
        ));
    }

    self.chunks.set_voxel(pos, value);
}

}  // namespace tmine
