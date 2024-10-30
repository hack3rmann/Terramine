#include "../terrain.hpp"

namespace tmine {

static auto add_vertex(std::vector<f32>* buffer_ptr, std::array<f32, 12> elems)
    -> void {
    buffer_ptr->insert(buffer_ptr->end(), elems.begin(), elems.end());
}

static auto has_opaque_block(
    ChunkArray const& chunks, GameBlocksData const& data, glm::uvec3 pos
) -> bool {
    auto id = chunks.get_voxel(pos);
    return id.has_value() && !data.blocks[(usize) id.value()].is_translucent();
}

TerrainRenderer::TerrainRenderer(GameBlocksData data) noexcept
: data{std::move(data)} {}

auto TerrainRenderer::render(
    this TerrainRenderer const& self, ChunkArray const& chunks, glm::uvec3 pos,
    Mesh* result_mesh
) -> void {
    if (nullptr == result_mesh) {
        return;
    }

    auto chunk = chunks.chunk(pos);

    if (nullptr == chunk) {
        return;
    }

    f32 ao_factor = 0.15f;

    auto& buffer = result_mesh->get_buffer();

    for (u32 y = 0; y < Chunk::HEIGHT; y++) {
        for (u32 z = 0; z < Chunk::DEPTH; z++) {
            for (u32 x = 0; x < Chunk::WIDTH; x++) {
                // Chunk always has voxel with coordinates (x, y, z)
                auto id = chunk->get_voxel({x, y, z}).value();

                if (0 == id) {
                    continue;
                }

                auto const& data = self.data.blocks[(usize) id];

                auto const tid = data.texture_ids[GameBlock::TOP_TEXTURE_INDEX];
                auto const boid =
                    data.texture_ids[GameBlock::BOTTOM_TEXTURE_INDEX];
                auto const lid =
                    data.texture_ids[GameBlock::LEFT_TEXTURE_INDEX];
                auto const rid =
                    data.texture_ids[GameBlock::RIGHT_TEXTURE_INDEX];
                auto const fid =
                    data.texture_ids[GameBlock::FRONT_TEXTURE_INDEX];
                auto const baid =
                    data.texture_ids[GameBlock::BACK_TEXTURE_INDEX];

                f32 l;
                f32 uvsize = 1.0f / 16.0f;

                f32 tu1 = (tid % 16) * uvsize;
                f32 tv1 = 1.0f - ((1.0f + tid / 16.0f) * uvsize);
                f32 tu2 = tu1 + uvsize;
                f32 tv2 = tv1 + uvsize;

                f32 bou1 = (boid % 16) * uvsize;
                f32 bov1 = 1.0f - ((1.0f + boid / 16.0f) * uvsize);
                f32 bou2 = bou1 + uvsize;
                f32 bov2 = bov1 + uvsize;

                f32 lu1 = (lid % 16) * uvsize;
                f32 lv1 = 1.0f - ((1.0f + lid / 16.0f) * uvsize);
                f32 lu2 = lu1 + uvsize;
                f32 lv2 = lv1 + uvsize;

                f32 ru1 = (rid % 16) * uvsize;
                f32 rv1 = 1.0f - ((1.0f + rid / 16.0f) * uvsize);
                f32 ru2 = ru1 + uvsize;
                f32 rv2 = rv1 + uvsize;

                f32 fu1 = (fid % 16) * uvsize;
                f32 fv1 = 1.0f - ((1.0f + fid / 16.0f) * uvsize);
                f32 fu2 = fu1 + uvsize;
                f32 fv2 = fv1 + uvsize;

                f32 bau1 = (baid % 16) * uvsize;
                f32 bav1 = 1.0f - ((1.0f + baid / 16.0f) * uvsize);
                f32 bau2 = bau1 + uvsize;
                f32 bav2 = bav1 + uvsize;

                /* Ambient Occlusion values */
                f32 a, b, c, d, e, f, g, h;
                a = b = c = d = e = f = g = h = 0.0f;

                has_opaque_block(chunks, self.data, pos);

                if (!has_opaque_block(chunks, self.data, {x, y + 1, z})) {
                    l = 1.0f;

                    if (TerrainRenderer::DO_AMBIENT_OCCLUSION) {
                        a = has_opaque_block(chunks, self.data, {x + 1, y + 1, z}) * ao_factor;
                        b = has_opaque_block(chunks, self.data, {x, y + 1, z + 1}) * ao_factor;
                        c = has_opaque_block(chunks, self.data, {x - 1, y + 1, z}) * ao_factor;
                        d = has_opaque_block(chunks, self.data, {x, y + 1, z - 1}) * ao_factor;

                        e = has_opaque_block(chunks, self.data, {x - 1, y + 1, z - 1}) * ao_factor;
                        f = has_opaque_block(chunks, self.data, {x - 1, y + 1, z + 1}) * ao_factor;
                        g = has_opaque_block(chunks, self.data, {x + 1, y + 1, z + 1}) * ao_factor;
                        h = has_opaque_block(chunks, self.data, {x + 1, y + 1, z - 1}) * ao_factor;
                    }

                    add_vertex(
                        &buffer,
                        {x - 0.5f, y + 0.5f, z - 0.5f, 0.0f, 1.0f, 0.0f, tu2,
                         tv1, l * (1.0f - c - d - e), 1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {x - 0.5f, y + 0.5f, z + 0.5f, 0.0f, 1.0f, 0.0f, tu2,
                         tv2, l * (1.0f - c - b - f), 1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {x + 0.5f, y + 0.5f, z + 0.5f, 0.0f, 1.0f, 0.0f, tu1,
                         tv2, l * (1.0f - a - b - g), 1.0f, 0.0f, 0.0f}
                    );

                    add_vertex(
                        &buffer,
                        {x - 0.5f, y + 0.5f, z - 0.5f, 0.0f, 1.0f, 0.0f, tu2,
                         tv1, l * (1.0f - c - d - e), 1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {x + 0.5f, y + 0.5f, z + 0.5f, 0.0f, 1.0f, 0.0f, tu1,
                         tv2, l * (1.0f - a - b - g), 1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {x + 0.5f, y + 0.5f, z - 0.5f, 0.0f, 1.0f, 0.0f, tu1,
                         tv1, l * (1.0f - a - d - h), 1.0f, 0.0f, 0.0f}
                    );
                }
                if (!has_opaque_block(chunks, self.data, {x, y - 1, z})) {
                    l = 0.75f;

                    if (TerrainRenderer::DO_AMBIENT_OCCLUSION) {
                        a = has_opaque_block(chunks, self.data, {x + 1, y - 1, z}) * ao_factor;
                        b = has_opaque_block(chunks, self.data, {x, y - 1, z + 1}) * ao_factor;
                        c = has_opaque_block(chunks, self.data, {x - 1, y - 1, z}) * ao_factor;
                        d = has_opaque_block(chunks, self.data, {x, y - 1, z - 1}) * ao_factor;

                        e = has_opaque_block(chunks, self.data, {x - 1, y - 1, z - 1}) * ao_factor;
                        f = has_opaque_block(chunks, self.data, {x - 1, y - 1, z + 1}) * ao_factor;
                        g = has_opaque_block(chunks, self.data, {x + 1, y - 1, z + 1}) * ao_factor;
                        h = has_opaque_block(chunks, self.data, {x + 1, y - 1, z - 1}) * ao_factor;
                    }

                    add_vertex(
                        &buffer,
                        {x - 0.5f, y - 0.5f, z - 0.5f, 0.0f, -1.0f, 0.0f, bou1,
                         bov1, l * (1.0f - c - d - e), 1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {x + 0.5f, y - 0.5f, z + 0.5f, 0.0f, -1.0f, 0.0f, bou2,
                         bov2, l * (1.0f - a - b - g), 1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {x - 0.5f, y - 0.5f, z + 0.5f, 0.0f, -1.0f, 0.0f, bou1,
                         bov2, l * (1.0f - c - b - f), 1.0f, 0.0f, 0.0f}
                    );

                    add_vertex(
                        &buffer,
                        {x - 0.5f, y - 0.5f, z - 0.5f, 0.0f, -1.0f, 0.0f, bou1,
                         bov1, l * (1.0f - c - d - e), 1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {x + 0.5f, y - 0.5f, z - 0.5f, 0.0f, -1.0f, 0.0f, bou2,
                         bov1, l * (1.0f - a - d - h), 1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {x + 0.5f, y - 0.5f, z + 0.5f, 0.0f, -1.0f, 0.0f, bou2,
                         bov2, l * (1.0f - a - b - g), 1.0f, 0.0f, 0.0f}
                    );
                }

                if (!has_opaque_block(chunks, self.data, {x + 1, y, z})) {
                    l = 0.95f;

                    if (TerrainRenderer::DO_AMBIENT_OCCLUSION) {
                        a = has_opaque_block(chunks, self.data, {x + 1, y + 1, z}) * ao_factor;
                        b = has_opaque_block(chunks, self.data, {x + 1, y, z + 1}) * ao_factor;
                        c = has_opaque_block(chunks, self.data, {x + 1, y - 1, z}) * ao_factor;
                        d = has_opaque_block(chunks, self.data, {x + 1, y, z - 1}) * ao_factor;

                        e = has_opaque_block(chunks, self.data, {x + 1, y - 1, z - 1}) * ao_factor;
                        f = has_opaque_block(chunks, self.data, {x + 1, y - 1, z + 1}) * ao_factor;
                        g = has_opaque_block(chunks, self.data, {x + 1, y + 1, z + 1}) * ao_factor;
                        h = has_opaque_block(chunks, self.data, {x + 1, y + 1, z - 1}) * ao_factor;
                    }

                    add_vertex(
                        &buffer,
                        {x + 0.5f, y - 0.5f, z - 0.5f, 1.0f, 0.0f, 0.0f, ru2,
                         rv1, l * (1.0f - c - d - e), 0.0f, 0.0f, -1.0f}
                    );
                    add_vertex(
                        &buffer,
                        {x + 0.5f, y + 0.5f, z - 0.5f, 1.0f, 0.0f, 0.0f, ru2,
                         rv2, l * (1.0f - d - a - h), 0.0f, 0.0f, -1.0f}
                    );
                    add_vertex(
                        &buffer,
                        {x + 0.5f, y + 0.5f, z + 0.5f, 1.0f, 0.0f, 0.0f, ru1,
                         rv2, l * (1.0f - a - b - g), 0.0f, 0.0f, -1.0f}
                    );

                    add_vertex(
                        &buffer,
                        {x + 0.5f, y - 0.5f, z - 0.5f, 1.0f, 0.0f, 0.0f, ru2,
                         rv1, l * (1.0f - c - d - e), 0.0f, 0.0f, -1.0f}
                    );
                    add_vertex(
                        &buffer,
                        {x + 0.5f, y + 0.5f, z + 0.5f, 1.0f, 0.0f, 0.0f, ru1,
                         rv2, l * (1.0f - a - b - g), 0.0f, 0.0f, -1.0f}
                    );
                    add_vertex(
                        &buffer,
                        {x + 0.5f, y - 0.5f, z + 0.5f, 1.0f, 0.0f, 0.0f, ru1,
                         rv1, l * (1.0f - b - c - f), 0.0f, 0.0f, -1.0f}
                    );
                }
                if (!has_opaque_block(chunks, self.data, {x - 1, y, z})) {
                    l = 0.85f;

                    if (TerrainRenderer::DO_AMBIENT_OCCLUSION) {
                        a = has_opaque_block(chunks, self.data, {x - 1, y + 1, z}) * ao_factor;
                        b = has_opaque_block(chunks, self.data, {x - 1, y, z + 1}) * ao_factor;
                        c = has_opaque_block(chunks, self.data, {x - 1, y - 1, z}) * ao_factor;
                        d = has_opaque_block(chunks, self.data, {x - 1, y, z - 1}) * ao_factor;

                        e = has_opaque_block(chunks, self.data, {x - 1, y - 1, z - 1}) * ao_factor;
                        f = has_opaque_block(chunks, self.data, {x - 1, y - 1, z + 1}) * ao_factor;
                        g = has_opaque_block(chunks, self.data, {x - 1, y + 1, z + 1}) * ao_factor;
                        h = has_opaque_block(chunks, self.data, {x - 1, y + 1, z - 1}) * ao_factor;
                    }

                    add_vertex(
                        &buffer,
                        {x - 0.5f, y - 0.5f, z - 0.5f, -1.0f, 0.0f, 0.0f, lu1,
                         lv1, l * (1.0f - c - d - e), 0.0f, 0.0f, 1.0f}
                    );
                    add_vertex(
                        &buffer,
                        {x - 0.5f, y + 0.5f, z + 0.5f, -1.0f, 0.0f, 0.0f, lu2,
                         lv2, l * (1.0f - a - b - g), 0.0f, 0.0f, 1.0f}
                    );
                    add_vertex(
                        &buffer,
                        {x - 0.5f, y + 0.5f, z - 0.5f, -1.0f, 0.0f, 0.0f, lu1,
                         lv2, l * (1.0f - d - a - h), 0.0f, 0.0f, 1.0f}
                    );

                    add_vertex(
                        &buffer,
                        {x - 0.5f, y - 0.5f, z - 0.5f, -1.0f, 0.0f, 0.0f, lu1,
                         lv1, l * (1.0f - c - d - e), 0.0f, 0.0f, 1.0f}
                    );
                    add_vertex(
                        &buffer,
                        {x - 0.5f, y - 0.5f, z + 0.5f, -1.0f, 0.0f, 0.0f, lu2,
                         lv1, l * (1.0f - b - c - f), 0.0f, 0.0f, 1.0f}
                    );
                    add_vertex(
                        &buffer,
                        {x - 0.5f, y + 0.5f, z + 0.5f, -1.0f, 0.0f, 0.0f, lu2,
                         lv2, l * (1.0f - a - b - g), 0.0f, 0.0f, 1.0f}
                    );
                }

                if (!has_opaque_block(chunks, self.data, {x, y, z + 1})) {
                    l = 0.9f;

                    if (TerrainRenderer::DO_AMBIENT_OCCLUSION) {
                        a = has_opaque_block(chunks, self.data, {x, y + 1, z + 1}) * ao_factor;
                        b = has_opaque_block(chunks, self.data, {x + 1, y, z + 1}) * ao_factor;
                        c = has_opaque_block(chunks, self.data, {x, y - 1, z + 1}) * ao_factor;
                        d = has_opaque_block(chunks, self.data, {x - 1, y, z + 1}) * ao_factor;

                        e = has_opaque_block(chunks, self.data, {x - 1, y - 1, z + 1}) * ao_factor;
                        f = has_opaque_block(chunks, self.data, {x + 1, y - 1, z + 1}) * ao_factor;
                        g = has_opaque_block(chunks, self.data, {x + 1, y + 1, z + 1}) * ao_factor;
                        h = has_opaque_block(chunks, self.data, {x - 1, y + 1, z + 1}) * ao_factor;
                    }

                    add_vertex(
                        &buffer,
                        {x - 0.5f, y - 0.5f, z + 0.5f, 0.0f, 0.0f, 1.0f, bau1,
                         bav1, l * (1.0f - c - d - e), 1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {x + 0.5f, y + 0.5f, z + 0.5f, 0.0f, 0.0f, 1.0f, bau2,
                         bav2, l * (1.0f - a - b - g), 1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {x - 0.5f, y + 0.5f, z + 0.5f, 0.0f, 0.0f, 1.0f, bau1,
                         bav2, l * (1.0f - a - d - h), 1.0f, 0.0f, 0.0f}
                    );

                    add_vertex(
                        &buffer,
                        {x - 0.5f, y - 0.5f, z + 0.5f, 0.0f, 0.0f, 1.0f, bau1,
                         bav1, l * (1.0f - c - d - e), 1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {x + 0.5f, y - 0.5f, z + 0.5f, 0.0f, 0.0f, 1.0f, bau2,
                         bav1, l * (1.0f - b - c - f), 1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {x + 0.5f, y + 0.5f, z + 0.5f, 0.0f, 0.0f, 1.0f, bau2,
                         bav2, l * (1.0f - a - b - g), 1.0f, 0.0f, 0.0f}
                    );
                }
                if (!has_opaque_block(chunks, self.data, {x, y, z - 1})) {
                    l = 0.8f;

                    if (TerrainRenderer::DO_AMBIENT_OCCLUSION) {
                        a = has_opaque_block(chunks, self.data, {x, y + 1, z - 1}) * ao_factor;
                        b = has_opaque_block(chunks, self.data, {x + 1, y, z - 1}) * ao_factor;
                        c = has_opaque_block(chunks, self.data, {x, y - 1, z - 1}) * ao_factor;
                        d = has_opaque_block(chunks, self.data, {x - 1, y, z - 1}) * ao_factor;

                        e = has_opaque_block(chunks, self.data, {x - 1, y - 1, z - 1}) * ao_factor;
                        f = has_opaque_block(chunks, self.data, {x + 1, y - 1, z - 1}) * ao_factor;
                        g = has_opaque_block(chunks, self.data, {x + 1, y + 1, z - 1}) * ao_factor;
                        h = has_opaque_block(chunks, self.data, {x - 1, y + 1, z - 1}) * ao_factor;
                    }

                    add_vertex(
                        &buffer,
                        {x - 0.5f, y - 0.5f, z - 0.5f, 0.0f, 0.0f, -1.0f, fu2,
                         fv1, l * (1.0f - c - d - e), -1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {x - 0.5f, y + 0.5f, z - 0.5f, 0.0f, 0.0f, -1.0f, fu2,
                         fv2, l * (1.0f - a - d - h), -1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {x + 0.5f, y + 0.5f, z - 0.5f, 0.0f, 0.0f, -1.0f, fu1,
                         fv2, l * (1.0f - a - b - g), -1.0f, 0.0f, 0.0f}
                    );

                    add_vertex(
                        &buffer,
                        {x - 0.5f, y - 0.5f, z - 0.5f, 0.0f, 0.0f, -1.0f, fu2,
                         fv1, l * (1.0f - c - d - e), -1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {x + 0.5f, y + 0.5f, z - 0.5f, 0.0f, 0.0f, -1.0f, fu1,
                         fv2, l * (1.0f - a - b - g), -1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {x + 0.5f, y - 0.5f, z - 0.5f, 0.0f, 0.0f, -1.0f, fu1,
                         fv1, l * (1.0f - b - c - f), -1.0f, 0.0f, 0.0f}
                    );
                }
            }
        }
    }

    result_mesh->reload_buffer();
}


}  // namespace tmine
