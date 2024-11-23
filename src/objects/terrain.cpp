#include <ranges>
#include <cstdlib>

#include "../objects.hpp"
#include "../loaders.hpp"
#include "../window.hpp"
#include "../debug.hpp"

namespace tmine {

namespace rg = std::ranges;
namespace vs = std::ranges::views;

// Idiomatic dedup algorithm taken from
// <https://www.geeksforgeeks.org/remove-duplicates-from-vector-in-cpp/>
template <class T>
static auto dedup_vector(RefMut<std::vector<T>> vec) -> void {
    rg::sort(*vec);
    auto it = std::unique(vec->begin(), vec->end());
    vec->erase(it, vec->end());
}

Terrain::Terrain(glm::uvec3 sizes)
: chunks{std::make_shared<ChunkArray>(sizes)}
, meshes{std::make_unique<Mesh<TerrainRenderer::Vertex>[]>(
      sizes.x * sizes.y * sizes.z
  )}
, chunks_to_update(sizes.x * sizes.y * sizes.z)
, chunks_with_transparency{}
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

    rg::sort(triangles, manhattan_comparator);
}

auto Terrain::generate_meshes(this Terrain& self, glm::vec3 camera_pos)
    -> void {
    // remove duplicates from vector to prevent data race
    dedup_vector(&self.chunks_to_update);
    dedup_vector(&self.chunks_with_transparency);

    self.transparent_mesh.get_buffer().clear();

#pragma omp parallel for
    for (auto i : self.chunks_to_update) {
        auto const pos = self.chunks->index_to_pos(i);

        // Do not reload mesh buffer on multithread
        self.renderer.render_opaque(
            *self.chunks, pos, &self.meshes[i], TerrainRenderUploadMesh::Skip
        );
    }

#pragma omp parallel for
    for (auto i : self.chunks_with_transparency) {
        auto const pos = self.chunks->index_to_pos(i);
        auto const chunk = *self.chunks->chunk(pos);

        self.renderer.render_transparent(
            chunk, *self.chunks, &self.transparent_mesh
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

auto Terrain::setup_render_resources(
    this Terrain& self, ShaderProgram const& shader, Camera const& camera,
    SceneParameters const& params, glm::uvec2 viewport_size
) -> void {
    shader.bind();
    shader.uniform_mat4(
        "projection",
        camera.get_projection(Window::aspect_ratio_of(viewport_size))
    );
    shader.uniform_mat4("view", camera.get_view());
    shader.uniform_vec2("resolution", glm::vec2{viewport_size});
    shader.uniform_vec3("to_light", -params.light_direction);
    shader.uniform_vec3("light_color", glm::vec3(0.96f, 0.24f, 0.0f));
    shader.uniform_int("albedo_texture", 0);
    shader.uniform_int("normal_texture", 1);

    self.texture_atlas.bind(0);
    self.normal_atlas.bind(1);
}

auto Terrain::render_transparent(
    this Terrain& self, Camera const& camera, SceneParameters const& params,
    glm::uvec2 viewport_size
) -> void {
    self.setup_render_resources(
        self.transparent_shader, camera, params, viewport_size
    );
    self.transparent_mesh.draw();
}

auto Terrain::render_opaque(
    this Terrain& self, Camera const& camera, SceneParameters const& params,
    glm::uvec2 viewport_size
) -> void {
    self.setup_render_resources(
        self.opaque_shader, camera, params, viewport_size
    );

    auto const n_chunks = self.chunks->chunk_count();
    auto model = glm::mat4{1.0f};
    auto meshes = std::span{self.meshes.get(), self.meshes.get() + n_chunks};

    for (auto [chunk, mesh] : vs::zip(self.chunks->as_span(), meshes)) {
        auto const pos = chunk.get_pos();
        auto const offset =
            glm::vec3{pos} * glm::vec3{Chunk::SIZE} + glm::vec3{0.5f};

        model = glm::translate(glm::mat4{1.0f}, offset);

        self.opaque_shader.uniform_mat4("model", model);
        mesh.draw();
    }
}

auto Terrain::set_voxel(this Terrain& self, glm::uvec3 pos, Voxel value)
    -> void {
    auto const chunk_pos = pos / Chunk::SIZE;
    auto const local_pos = pos % Chunk::SIZE;

    if (!self.chunks->is_in_bounds(chunk_pos) ||
        !Chunk::is_in_bounds(local_pos))
    {
        return;
    }

    auto chunk_index = self.chunks->index_of(chunk_pos);
    auto prev_voxel_id = self.chunks->get_voxel(pos).value();

    self.chunks->set_voxel(pos, value);

    // update `chunks_with_transparency` if user is removing transparent voxel
    if (0 == value.id &&
        self.renderer.data.blocks[prev_voxel_id.id][0].is_translucent())
    {
        auto& chunk = *self.chunks->chunk(chunk_pos);
        bool contains_transparent =
            rg::any_of(chunk.get_voxels(), [&self](auto voxel) {
                return 0 != voxel.id &&
                       self.renderer.data.blocks[voxel.id][0].is_translucent();
            });

        // remove chunk which is transparent no more
        if (!contains_transparent && !self.chunks_with_transparency.empty()) {
            auto const iter =
                rg::lower_bound(self.chunks_with_transparency, chunk_index);

            rg::iter_swap(iter, self.chunks_with_transparency.end() - 1);
            self.chunks_with_transparency.pop_back();
        }
    }

    if (0 != value.id &&
        self.renderer.data.blocks[value.id][0].is_translucent())
    {
        self.chunks_with_transparency.push_back(chunk_index);
        return;
    }

    auto const sizes = self.chunks->size();

    self.chunks_to_update.push_back(self.chunks->index_of(chunk_pos));

    if (0 == local_pos.x && 0 != chunk_pos.x) {
        self.chunks_to_update.push_back(self.chunks->index_of(
            glm::uvec3(chunk_pos.x - 1, chunk_pos.y, chunk_pos.z)
        ));
    }

    if (Chunk::WIDTH == local_pos.x + 1 && sizes.x != chunk_pos.x + 1) {
        self.chunks_to_update.push_back(self.chunks->index_of(
            glm::uvec3(chunk_pos.x + 1, chunk_pos.y, chunk_pos.z)
        ));
    }

    if (0 == local_pos.y && 0 != chunk_pos.y) {
        self.chunks_to_update.push_back(self.chunks->index_of(
            glm::uvec3(chunk_pos.x, chunk_pos.y - 1, chunk_pos.z)
        ));
    }

    if (Chunk::HEIGHT == local_pos.y + 1 && sizes.y != chunk_pos.y + 1) {
        self.chunks_to_update.push_back(self.chunks->index_of(
            glm::uvec3(chunk_pos.x, chunk_pos.y + 1, chunk_pos.z)
        ));
    }

    if (0 == local_pos.z && 0 != chunk_pos.z) {
        self.chunks_to_update.push_back(self.chunks->index_of(
            glm::uvec3(chunk_pos.x, chunk_pos.y, chunk_pos.z - 1)
        ));
    }

    if (Chunk::DEPTH == local_pos.z + 1 && sizes.z != chunk_pos.z + 1) {
        self.chunks_to_update.push_back(self.chunks->index_of(
            glm::uvec3(chunk_pos.x, chunk_pos.y, chunk_pos.z + 1)
        ));
    }
}

auto TerrainCollider::get_collidable_bounding_box() const -> Aabb {
    return Aabb{
        glm::vec3{0.0f},
        glm::vec3{this->chunks->size() * Chunk::SIZE},
    };
}

auto TerrainCollider::collide(Collidable const& other) const -> Collision {
    if (auto other_box = dynamic_cast<BoxCollider const*>(&other)) {
        return this->collide_box(*other_box);
    } else {
        return Collision{};
    }
}

auto TerrainCollider::collide_box(
    this TerrainCollider const& self, BoxCollider const& other
) -> Collision {
    auto const other_box = other.get_collidable_bounding_box();
    auto const position_corrected_box = Aabb{
        other_box.lo - glm::vec3{0.5f},
        other_box.hi - glm::vec3{0.5f},
    };

    auto const lo = glm::uvec3{
        glm::max(glm::vec3{0.0f}, glm::round(position_corrected_box.lo))
    };

    auto const hi = glm::uvec3{
        glm::max(glm::vec3{0.0f}, glm::round(position_corrected_box.hi))
    };

    auto displacement = glm::vec3{0.0f};
    auto directional_counts = glm::ivec3{0};

    debug::lines()->box(other_box, 0.8f * DebugColor::GREEN);

    for (u32 x = lo.x; x <= hi.x; ++x) {
        for (u32 y = lo.y; y <= hi.y; ++y) {
            for (u32 z = lo.z; z <= hi.z; ++z) {
                auto maybe_id = self.chunks->get_voxel({x, y, z});

                if (!maybe_id.has_value() || 0 == maybe_id.value().id) {
                    continue;
                }

                auto const lo = glm::vec3{x, y, z};
                auto const box = Aabb{lo, lo + glm::vec3{1.0f}};

                debug::lines()->box(box, 0.8f * DebugColor::BLUE);

                auto const wall_collider = BoxCollider{
                    box,
                    glm::vec3{0.0f},
                    glm::vec3{0.0f},
                    ABSOLUTELY_ELASTIC_ELASTICITY,
                    false,
                };

                auto cur_displacement =
                    other.collide(wall_collider).self_displacement;

                directional_counts +=
                    glm::abs(glm::ivec3{glm::sign(cur_displacement)});

                displacement += cur_displacement;
            }
        }
    }

    directional_counts = glm::max(glm::ivec3{1}, directional_counts);
    displacement /= glm::vec3{directional_counts};

    return Collision{
        .self_displacement = glm::vec3{0.0f},
        .other_displacement = displacement,
    };
}

}  // namespace tmine
