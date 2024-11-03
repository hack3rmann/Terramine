#include <ranges>
#include <cstdlib>

#include "../objects.hpp"
#include "../loaders.hpp"
#include "../window.hpp"

namespace tmine {

namespace vs = std::ranges::views;

Terrain::Terrain(glm::uvec3 sizes)
: chunks{sizes}  // HACK(hack3rmann): there is no way in C++ to make array from
                 // a function that returns new elements
, meshes{(Mesh*) ::operator new(
      sizeof(this->meshes[0]) * sizes.x * sizes.y * sizes.z
  )}
, is_modified{new bool[sizes.x * sizes.y * sizes.z]}
, renderer{load_game_blocks_data(
      Terrain::BLOCK_DATA_PATH, Terrain::BLOCK_TEXTURE_DATA_PATH
  )}
, shader{load_shader(Terrain::VERTEX_SHADER_NAME, Terrain::FRAGMENT_SHADER_NAME)
  }
, texture_atlas{Texture::from_image(
      load_png(Terrain::TEXTURE_ATLAS_PATH), TextureLoad::DEFAULT
  )}
, normal_atlas{Texture::from_image(
      load_png(Terrain::NORMAL_ATLAS_PATH), TextureLoad::DEFAULT
  )} {
    auto const n_meshes = sizes.x * sizes.y * sizes.z;

    std::memset(this->is_modified.get(), true, n_meshes);

    // HACK(hac3rmann): so we need to manually call constructor on each element
    for (usize i = 0; i < n_meshes; ++i) {
        new (this->meshes.get() + i) Mesh{TerrainRenderer::make_empty_mesh()};
    }

    this->generate_meshes();
}

auto Terrain::generate_meshes(this Terrain& self) -> void {
    for (usize x = 0; x < self.chunks.get_sizes().x; ++x) {
        for (usize y = 0; y < self.chunks.get_sizes().y; ++y) {
            for (usize z = 0; z < self.chunks.get_sizes().z; ++z) {
                auto const pos = glm::uvec3{x, y, z};
                auto const index = self.chunks.index_of(pos);

                if (self.is_modified[index]) {
                    self.is_modified[index] = false;
                } else {
                    continue;
                }

                self.renderer.render(self.chunks, pos, &self.meshes[index]);
            }
        }
    }
}

auto Terrain::update(this Terrain& self) -> void { self.generate_meshes(); }

auto Terrain::render(
    this Terrain const& self, Camera const& cam, glm::uvec3 light_direction, glm::uvec2 window_size
) -> void {
    self.shader.bind();
    self.shader.uniform_mat4("proj", cam.get_projection(Window::aspect_ratio_of(window_size)));
    self.shader.uniform_mat4("view", cam.get_view());
    self.shader.uniform_vec2("resolution", glm::vec2{window_size});
    self.shader.uniform_vec3("toLightVec", -light_direction);
    self.shader.uniform_vec3("lightColor", glm::vec3(0.96f, 0.24f, 0.0f));
    self.shader.uniform_int("u_Texture0", 0);
    self.shader.uniform_int("u_Texture1", 1);

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

        self.shader.uniform_mat4("model", model);
        mesh.draw();
    }
}

auto Terrain::set_voxel(this Terrain& self, glm::uvec3 pos, VoxelId value)
    -> void {
    auto const chunk_pos = pos / Chunk::SIZES;
    auto const voxel_pos = pos % Chunk::SIZES;

    if (!self.chunks.is_in_bounds(chunk_pos) || !Chunk::is_in_bounds(voxel_pos))
    {
        return;
    }

    // TODO(hack3rmann): modify neighbors
    self.is_modified[self.chunks.index_of(chunk_pos)] = true;

    if (0 == voxel_pos.x && 0 != chunk_pos.x) {
        self.is_modified[self.chunks.index_of(
            glm::uvec3(chunk_pos.x - 1, chunk_pos.y, chunk_pos.z)
        )] = true;
    }

    if (Chunk::WIDTH == voxel_pos.x + 1 &&
        self.chunks.get_sizes().x != chunk_pos.x + 1)
    {
        self.is_modified[self.chunks.index_of(
            glm::uvec3(chunk_pos.x + 1, chunk_pos.y, chunk_pos.z)
        )] = true;
    }

    if (0 == voxel_pos.y && 0 != chunk_pos.y) {
        self.is_modified[self.chunks.index_of(
            glm::uvec3(chunk_pos.x, chunk_pos.y - 1, chunk_pos.z)
        )] = true;
    }

    if (Chunk::HEIGHT == voxel_pos.y + 1 &&
        self.chunks.get_sizes().y != chunk_pos.y + 1)
    {
        self.is_modified[self.chunks.index_of(
            glm::uvec3(chunk_pos.x, chunk_pos.y + 1, chunk_pos.z)
        )] = true;
    }

    if (0 == voxel_pos.z && 0 != chunk_pos.z) {
        self.is_modified[self.chunks.index_of(
            glm::uvec3(chunk_pos.x, chunk_pos.y, chunk_pos.z - 1)
        )] = true;
    }

    if (Chunk::DEPTH == voxel_pos.z + 1 &&
        self.chunks.get_sizes().z != chunk_pos.z + 1)
    {
        self.is_modified[self.chunks.index_of(
            glm::uvec3(chunk_pos.x, chunk_pos.y, chunk_pos.z + 1)
        )] = true;
    }

    self.chunks.set_voxel(pos, value);
}

}  // namespace tmine
