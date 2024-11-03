#pragma once

#include <array>
#include <optional>
#include <glm/glm.hpp>

#include "types.hpp"
#include "graphics.hpp"
#include "data.hpp"

namespace tmine {

class Chunk {
public:
    explicit Chunk(glm::uvec3 pos);

    static auto index_of(glm::uvec3 pos) noexcept -> usize;
    static auto is_in_bounds(glm::uvec3 pos) noexcept -> bool;

    auto get_voxel(this Chunk const& self, glm::uvec3 pos) noexcept
        -> std::optional<VoxelId>;

    auto set_voxel(this Chunk& self, glm::uvec3 pos, VoxelId id) noexcept
        -> void;

    inline auto get_pos(this Chunk const& self) noexcept -> glm::uvec3 {
        return self.pos;
    }

public:
    static auto constexpr N_POSITION_BITS = usize{4};
    static auto constexpr WIDTH = usize{1 << N_POSITION_BITS};
    static auto constexpr HEIGHT = usize{1 << N_POSITION_BITS};
    static auto constexpr DEPTH = usize{1 << N_POSITION_BITS};
    static auto constexpr SIZES = glm::uvec3{WIDTH, HEIGHT, DEPTH};
    static auto constexpr VOLUME = WIDTH * HEIGHT * DEPTH;

private:
    glm::uvec3 pos;
    std::array<VoxelId, VOLUME> voxel_ids;
};

struct RayCastResult {
    VoxelId id{0};
    glm::uvec3 voxel_pos{0};
    glm::vec3 hit_pos{0.0f};
    glm::vec3 normal{0.0f};
    bool has_hit{false};
};

class ChunkArray {
public:
    explicit ChunkArray(glm::uvec3 sizes);

    auto index_of(this ChunkArray const& self, glm::uvec3 chunk_pos) noexcept
        -> usize;

    auto index_to_pos(this ChunkArray const& self, usize index) noexcept
        -> glm::uvec3;

    auto is_in_bounds(
        this ChunkArray const& self, glm::uvec3 chunk_pos
    ) noexcept -> bool;

    auto chunk(this ChunkArray const& self, glm::uvec3 chunk_pos) noexcept
        -> Chunk const*;

    auto chunk(this ChunkArray& self, glm::uvec3 chunk_pos) noexcept -> Chunk*;

    auto get_voxel(this ChunkArray const& self, glm::uvec3 voxel_pos) noexcept
        -> std::optional<VoxelId>;

    auto set_voxel(
        this ChunkArray& self, glm::uvec3 voxel_pos, VoxelId value
    ) noexcept -> void;

    auto ray_cast(
        this ChunkArray const& self, glm::vec3 origin, glm::vec3 direction,
        f32 max_distance
    ) -> RayCastResult;

    inline auto get_sizes(this ChunkArray const& self) noexcept -> glm::uvec3 {
        return self.sizes;
    }

    inline auto get_volume(this ChunkArray const& self) noexcept -> usize {
        return self.sizes.x * self.sizes.y * self.sizes.z;
    }

    inline auto as_span(this ChunkArray const& self) noexcept
        -> std::span<Chunk const> {
        return std::span<Chunk const>{
            self.chunks.get(), self.chunks.get() + self.get_volume()
        };
    }

    inline auto get_span(this ChunkArray& self) noexcept -> std::span<Chunk> {
        return std::span<Chunk>{
            self.chunks.get(), self.chunks.get() + self.get_volume()
        };
    }

    inline auto chunk_count(this ChunkArray const& self) noexcept -> usize {
        return self.sizes.x * self.sizes.y * self.sizes.z;
    }

private:
    std::unique_ptr<Chunk[]> chunks;
    glm::uvec3 sizes;
};

auto height_map_at(glm::uvec2 pos) -> f32;

enum class TerrainRenderUploadMesh {
    DoUpload,
    Skip,
};

class TerrainRenderer {
public:
    explicit TerrainRenderer(GameBlocksData data) noexcept;

    auto render(
        this TerrainRenderer const& self, ChunkArray const& chunks,
        glm::uvec3 pos, Mesh* result_mesh,
        TerrainRenderUploadMesh upload = TerrainRenderUploadMesh::DoUpload
    ) -> void;

    static auto make_empty_mesh() -> Mesh;

public:
    static auto constexpr DO_AMBIENT_OCCLUSION = true;
    static auto constexpr VERTEX_ATTRIBUTE_SIZES =
        std::array<tmine::usize, 2>{1, 2};

private:
    GameBlocksData data;
};

}  // namespace tmine
