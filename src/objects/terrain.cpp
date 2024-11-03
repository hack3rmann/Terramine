#include <ranges>
#include <cstdlib>

#include "../objects.hpp"
#include "../loaders.hpp"
#include "../window.hpp"

namespace tmine {

namespace vs = std::ranges::views;

Terrain::Terrain(glm::uvec3 sizes)
: chunks{sizes}
, meshes{(Mesh<TerrainRenderer::Vertex>*) ::operator new(
      sizeof(this->meshes[0]) * sizes.x * sizes.y * sizes.z
  )}
, states{new ChunkState[sizes.x * sizes.y * sizes.z]}
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

    std::memset(this->states.get(), (u8) ChunkState::VoxelsUpdated, n_meshes);

    for (usize i = 0; i < n_meshes; ++i) {
        new (this->meshes.get() + i) Mesh{TerrainRenderer::make_empty_mesh()};
    }

    this->generate_meshes();
}

auto Terrain::generate_meshes(this Terrain& self) -> void {
#pragma omp parallel for
    for (usize i = 0; i < self.chunks.get_volume(); ++i) {
        auto const pos = self.chunks.index_to_pos(i);

        if (ChunkState::VoxelsUpdated == self.states[i]) {
            self.states[i] = ChunkState::MeshUpdated;
        } else {
            continue;
        }

        // Do not reload mesh buffer on multithread
        self.renderer.render(
            self.chunks, pos, &self.meshes[i], TerrainRenderUploadMesh::Skip
        );
    }

    // Reload buffers on main thread
    for (usize i = 0; i < self.chunks.get_volume(); ++i) {
        if (ChunkState::MeshUpdated == self.states[i]) {
            self.states[i] = ChunkState::UpToDate;
        }

        self.meshes[i].reload_buffer();
    }
}

auto Terrain::update(this Terrain& self) -> void { self.generate_meshes(); }

auto Terrain::render(
    this Terrain const& self, Camera const& cam, glm::uvec3 light_direction,
    glm::uvec2 window_size
) -> void {
    self.shader.bind();
    self.shader.uniform_mat4(
        "proj", cam.get_projection(Window::aspect_ratio_of(window_size))
    );
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

    self.states[self.chunks.index_of(chunk_pos)] = ChunkState::VoxelsUpdated;

    if (0 == voxel_pos.x && 0 != chunk_pos.x) {
        self.states[self.chunks.index_of(
            glm::uvec3(chunk_pos.x - 1, chunk_pos.y, chunk_pos.z)
        )] = ChunkState::VoxelsUpdated;
    }

    if (Chunk::WIDTH == voxel_pos.x + 1 &&
        self.chunks.get_sizes().x != chunk_pos.x + 1)
    {
        self.states[self.chunks.index_of(
            glm::uvec3(chunk_pos.x + 1, chunk_pos.y, chunk_pos.z)
        )] = ChunkState::VoxelsUpdated;
    }

    if (0 == voxel_pos.y && 0 != chunk_pos.y) {
        self.states[self.chunks.index_of(
            glm::uvec3(chunk_pos.x, chunk_pos.y - 1, chunk_pos.z)
        )] = ChunkState::VoxelsUpdated;
    }

    if (Chunk::HEIGHT == voxel_pos.y + 1 &&
        self.chunks.get_sizes().y != chunk_pos.y + 1)
    {
        self.states[self.chunks.index_of(
            glm::uvec3(chunk_pos.x, chunk_pos.y + 1, chunk_pos.z)
        )] = ChunkState::VoxelsUpdated;
    }

    if (0 == voxel_pos.z && 0 != chunk_pos.z) {
        self.states[self.chunks.index_of(
            glm::uvec3(chunk_pos.x, chunk_pos.y, chunk_pos.z - 1)
        )] = ChunkState::VoxelsUpdated;
    }

    if (Chunk::DEPTH == voxel_pos.z + 1 &&
        self.chunks.get_sizes().z != chunk_pos.z + 1)
    {
        self.states[self.chunks.index_of(
            glm::uvec3(chunk_pos.x, chunk_pos.y, chunk_pos.z + 1)
        )] = ChunkState::VoxelsUpdated;
    }

    self.chunks.set_voxel(pos, value);
}

}  // namespace tmine
