#include "../terrain.hpp"

namespace tmine {

ChunkArray::ChunkArray(glm::uvec3 sizes)
: chunks{(Chunk*) ::operator new[](
      sizeof(this->chunks[0]) * sizes.x * sizes.y * sizes.z
  )}
, sizes{sizes} {
    auto const volume = sizes.x * sizes.y * sizes.z;

#pragma omp parallel for
    for (usize i = 0; i < volume; ++i) {
        auto const pos = this->index_to_pos(i);

        new (this->chunks.get() + i) Chunk{pos};
    }
}

auto ChunkArray::index_of(this ChunkArray const& self, glm::uvec3 pos) noexcept
    -> usize {
    return (pos.y * self.sizes.z + pos.z) * self.sizes.x + pos.x;
}

auto ChunkArray::index_to_pos(this ChunkArray const& self, usize index) noexcept
    -> glm::uvec3 {
    usize x = index % self.sizes.x;
    usize zy = index / self.sizes.x;
    usize z = zy % self.sizes.z;
    usize y = zy / self.sizes.z;

    return glm::uvec3{x, y, z};
}

auto ChunkArray::is_in_bounds(
    this ChunkArray const& self, glm::uvec3 chunk_pos
) noexcept -> bool {
    return chunk_pos.x < self.sizes.x && chunk_pos.y < self.sizes.y &&
           chunk_pos.z < self.sizes.z;
}

auto ChunkArray::chunk(this ChunkArray const& self, glm::uvec3 pos) noexcept
    -> Chunk const* {
    if (!self.is_in_bounds(pos)) {
        return nullptr;
    }

    return &self.chunks[self.index_of(pos)];
}

auto ChunkArray::chunk(this ChunkArray& self, glm::uvec3 pos) noexcept
    -> Chunk* {
    if (!self.is_in_bounds(pos)) {
        return nullptr;
    }

    return &self.chunks[self.index_of(pos)];
}

auto ChunkArray::get_voxel(
    this ChunkArray const& self, glm::uvec3 voxel_pos
) noexcept -> std::optional<Voxel> {
    auto const chunk_pos = voxel_pos / Chunk::SIZE;
    auto const local_pos = voxel_pos % Chunk::SIZE;

    auto chunk = self.chunk(chunk_pos);

    if (nullptr == chunk) {
        return std::nullopt;
    }

    return chunk->get_voxel(local_pos);
}

auto ChunkArray::set_voxel(
    this ChunkArray& self, glm::uvec3 voxel_pos, Voxel value
) noexcept -> void {
    auto const chunk_pos = voxel_pos / Chunk::SIZE;
    auto const local_pos = voxel_pos % Chunk::SIZE;

    auto chunk = self.chunk(chunk_pos);

    if (nullptr == chunk) {
        return;
    }

    chunk->set_voxel(local_pos, value);
}

auto ChunkArray::ray_cast(
    this ChunkArray const& self, glm::vec3 origin, glm::vec3 direction,
    f32 max_distance
) -> RayCastResult {
    f32 px = origin.x;
    f32 py = origin.y;
    f32 pz = origin.z;

    f32 dx = direction.x;
    f32 dy = direction.y;
    f32 dz = direction.z;

    f32 t = 0.0f;
    i32 ix = floor(px);
    i32 iy = floor(py);
    i32 iz = floor(pz);

    i32 stepx = (dx > 0.0f) ? 1 : -1;
    i32 stepy = (dy > 0.0f) ? 1 : -1;
    i32 stepz = (dz > 0.0f) ? 1 : -1;

    f32 infinity = std::numeric_limits<f32>::infinity();

    f32 tx_delta = (dx == 0.0f) ? infinity : std::abs(1.0f / dx);
    f32 ty_delta = (dy == 0.0f) ? infinity : std::abs(1.0f / dy);
    f32 tz_delta = (dz == 0.0f) ? infinity : std::abs(1.0f / dz);

    f32 xdist = (stepx > 0) ? (ix + 1 - px) : (px - ix);
    f32 ydist = (stepy > 0) ? (iy + 1 - py) : (py - iy);
    f32 zdist = (stepz > 0) ? (iz + 1 - pz) : (pz - iz);

    f32 tx_max = (tx_delta < infinity) ? tx_delta * xdist : infinity;
    f32 ty_max = (ty_delta < infinity) ? ty_delta * ydist : infinity;
    f32 tz_max = (tz_delta < infinity) ? tz_delta * zdist : infinity;

    i32 stepped_index = -1;

    while (t <= max_distance) {
        auto voxel = self.get_voxel({ix, iy, iz});

        if (voxel.has_value() && 0 != voxel.value().id) {
            auto normal = glm::vec3{0.0f};

            switch (stepped_index) {
            case 0:
                normal.x = -stepx;
                break;
            case 1:
                normal.y = -stepy;
                break;
            case 2:
                normal.z = -stepz;
                break;
            default:
                break;
            }

            return RayCastResult{
                .voxel = voxel.value(),
                .voxel_pos = {ix, iy, iz},
                .hit_pos = glm::vec3{px + t * dx, py + t * dy, pz + t * dz},
                .normal = normal,
                .has_hit = true,
            };
        }

        if (tx_max < ty_max) {
            if (tx_max < tz_max) {
                ix += stepx;
                t = tx_max;
                tx_max += tx_delta;
                stepped_index = 0;
            } else {
                iz += stepz;
                t = tz_max;
                tz_max += tz_delta;
                stepped_index = 2;
            }
        } else {
            if (ty_max < tz_max) {
                iy += stepy;
                t = ty_max;
                ty_max += ty_delta;
                stepped_index = 1;
            } else {
                iz += stepz;
                t = tz_max;
                tz_max += tz_delta;
                stepped_index = 2;
            }
        }
    }

    return RayCastResult{
        .voxel_pos = glm::ivec3{ix, iy, iz},
        .hit_pos = glm::vec3{px + t * dx, py + t * dy, pz + t * dz},
        .has_hit = false,
    };
}

}  // namespace tmine
