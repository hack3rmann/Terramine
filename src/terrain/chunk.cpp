#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

#include "../terrain.hpp"

namespace tmine {

Chunk::Chunk(glm::uvec3 chunk_pos)
: pos{chunk_pos}
, voxel_ids{} {
    for (usize local_z = 0; local_z < Chunk::DEPTH; local_z++) {
        for (usize local_x = 0; local_x < Chunk::WIDTH; local_x++) {
            auto const world_x = local_x + chunk_pos.x * Chunk::WIDTH;
            auto const world_z = local_z + chunk_pos.z * Chunk::DEPTH;
            auto const height = height_map_at({world_x, world_z});

            for (usize local_y = 0; local_y < Chunk::HEIGHT; local_y++) {
                auto const world_y = local_y + chunk_pos.y * Chunk::HEIGHT;
                auto const sample_height = (usize) (30.0f * height);

                auto id = VoxelId{0};

                if (world_y <= sample_height + 39) {
                    id = 2;
                } else if (world_y <= sample_height + 40) {
                    id = 1;
                }

                this->set_voxel({local_x, local_y, local_z}, id);
            }
        }
    }
}

auto Chunk::index_of(glm::uvec3 pos) noexcept -> usize {
    return (pos.y * Chunk::DEPTH + pos.z) * Chunk::WIDTH + pos.x;
}

auto Chunk::is_in_bounds(glm::uvec3 pos) noexcept -> bool {
    return pos.x < Chunk::WIDTH && pos.y < Chunk::HEIGHT &&
           pos.z < Chunk::DEPTH;
}

auto Chunk::get_voxel(this Chunk const& self, glm::uvec3 pos) noexcept
    -> std::optional<VoxelId> {
    if (!Chunk::is_in_bounds(pos)) {
        return std::nullopt;
    }

    return self.voxel_ids[Chunk::index_of(pos)];
}

auto Chunk::set_voxel(this Chunk& self, glm::uvec3 pos, VoxelId id) noexcept -> void {
    if (!Chunk::is_in_bounds(pos)) {
        return;
    }

    self.voxel_ids[Chunk::index_of(pos)] = id;
}

auto height_map_at(glm::uvec2 pos) -> f32 {
    auto const pos_ext = glm::vec3{pos, 0.0f};

    auto const layers = std::array<glm::vec3, 4>{
        0.0026125f * pos_ext,
        0.006125f * pos_ext,
        0.018125f * pos_ext,
        0.038125f * pos_ext,
    };

    return glm::perlin(layers[0]) + 0.5f * glm::perlin(layers[1]) +
           0.25f * glm::perlin(layers[2]) + 0.175f * glm::perlin(layers[3]);
}

}  // namespace tmine
