#pragma once

#include <array>
#include <optional>
#include <glm/glm.hpp>

#include "types.hpp"

namespace tmine {

using VoxelId = u8;

class Chunk {
public:
    explicit Chunk(glm::uvec3 pos);

    static auto index_of(glm::uvec3 pos) noexcept -> usize;
    static auto is_in_bounds(glm::uvec3 pos) noexcept -> bool;

    auto get_voxel(this Chunk const& self, glm::uvec3 pos) noexcept
        -> std::optional<VoxelId>;

    auto set_voxel(this Chunk& self, glm::uvec3 pos, VoxelId id) noexcept
        -> void;

public:
    static auto constexpr WIDTH = usize{8};
    static auto constexpr HEIGHT = usize{8};
    static auto constexpr DEPTH = usize{8};
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

private:
    std::vector<Chunk> chunks;
    glm::uvec3 sizes;
};

auto height_map_at(glm::uvec2 pos) -> f32;

}  // namespace tmine
