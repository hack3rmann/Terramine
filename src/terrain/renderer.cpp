#include "../terrain.hpp"

namespace tmine {

auto constexpr POS_X_NORMAL = u32{0};
auto constexpr NEG_X_NORMAL = u32{1};
auto constexpr POS_Y_NORMAL = u32{2};
auto constexpr NEG_Y_NORMAL = u32{3};
auto constexpr POS_Z_NORMAL = u32{4};
auto constexpr NEG_Z_NORMAL = u32{5};

static auto encode_data(glm::uvec3 pos, u32 offset, u32 normal) -> f32 {
    static_assert(
        Chunk::N_POSITION_BITS == 4, "only 4 bit position is supported"
    );

    auto constexpr N_OFFSET_BITS = 3;
    auto result = u32{0};

    result |= pos.x << (0 * Chunk::N_POSITION_BITS);
    result |= pos.y << (1 * Chunk::N_POSITION_BITS);
    result |= pos.z << (2 * Chunk::N_POSITION_BITS);
    result |= offset << (3 * Chunk::N_POSITION_BITS);
    result |= normal << (3 * Chunk::N_POSITION_BITS + N_OFFSET_BITS);

    return std::bit_cast<f32>(result);
}

static auto add_vertex(std::vector<f32>* buffer_ptr, std::array<f32, 7> elems)
    -> void {
    buffer_ptr->insert(buffer_ptr->end(), elems.begin(), elems.end());
}

static auto is_opaque(
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
    buffer.clear();

    for (u32 y = 0; y < Chunk::HEIGHT; y++) {
        for (u32 z = 0; z < Chunk::DEPTH; z++) {
            for (u32 x = 0; x < Chunk::WIDTH; x++) {
                // Chunk always has voxel with coordinates (x, y, z)
                auto id = chunk->get_voxel({x, y, z}).value();

                if (0 == id) {
                    continue;
                }

                auto global_offset =
                    glm::ivec3{Chunk::SIZES * chunk->get_pos()};

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

                auto tidd = tid / 16;
                f32 tu1 = (tid % 16) * uvsize;
                f32 tv1 = 1.0f - ((1.0f + tidd) * uvsize);
                f32 tu2 = tu1 + uvsize;
                f32 tv2 = tv1 + uvsize;

                auto boidd = boid / 16;
                f32 bou1 = (boid % 16) * uvsize;
                f32 bov1 = 1.0f - ((1.0f + boidd) * uvsize);
                f32 bou2 = bou1 + uvsize;
                f32 bov2 = bov1 + uvsize;

                auto lidd = lid / 16;
                f32 lu1 = (lid % 16) * uvsize;
                f32 lv1 = 1.0f - ((1.0f + lidd) * uvsize);
                f32 lu2 = lu1 + uvsize;
                f32 lv2 = lv1 + uvsize;

                auto ridd = rid / 16;
                f32 ru1 = (rid % 16) * uvsize;
                f32 rv1 = 1.0f - ((1.0f + ridd) * uvsize);
                f32 ru2 = ru1 + uvsize;
                f32 rv2 = rv1 + uvsize;

                auto fidd = fid / 16;
                f32 fu1 = (fid % 16) * uvsize;
                f32 fv1 = 1.0f - ((1.0f + fidd) * uvsize);
                f32 fu2 = fu1 + uvsize;
                f32 fv2 = fv1 + uvsize;

                auto baidd = baid / 16;
                f32 bau1 = (baid % 16) * uvsize;
                f32 bav1 = 1.0f - ((1.0f + baidd) * uvsize);
                f32 bau2 = bau1 + uvsize;
                f32 bav2 = bav1 + uvsize;

                /* Ambient Occlusion values */
                f32 a, b, c, d, e, f, g, h;
                a = b = c = d = e = f = g = h = 0.0f;

                if (!is_opaque(
                        chunks, self.data,
                        global_offset + glm::ivec3{x, y + 1, z}
                    ))
                {
                    l = 1.0f;

                    if (TerrainRenderer::DO_AMBIENT_OCCLUSION) {
                        a = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x + 1, y + 1, z}
                            ) *
                            ao_factor;
                        b = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x, y + 1, z + 1}
                            ) *
                            ao_factor;
                        c = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x - 1, y + 1, z}
                            ) *
                            ao_factor;
                        d = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x, y + 1, z - 1}
                            ) *
                            ao_factor;

                        e = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x - 1, y + 1, z - 1}
                            ) *
                            ao_factor;
                        f = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x - 1, y + 1, z + 1}
                            ) *
                            ao_factor;
                        g = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x + 1, y + 1, z + 1}
                            ) *
                            ao_factor;
                        h = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x + 1, y + 1, z - 1}
                            ) *
                            ao_factor;
                    }

                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b101, POS_Y_NORMAL), tu2,
                         tv1, l * (1.0f - c - d - e), 1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b100, POS_Y_NORMAL), tu2,
                         tv2, l * (1.0f - c - b - f), 1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b000, POS_Y_NORMAL), tu1,
                         tv2, l * (1.0f - a - b - g), 1.0f, 0.0f, 0.0f}
                    );

                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b101, POS_Y_NORMAL), tu2,
                         tv1, l * (1.0f - c - d - e), 1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b000, POS_Y_NORMAL), tu1,
                         tv2, l * (1.0f - a - b - g), 1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b001, POS_Y_NORMAL), tu1,
                         tv1, l * (1.0f - a - d - h), 1.0f, 0.0f, 0.0f}
                    );
                }
                if (!is_opaque(
                        chunks, self.data,
                        global_offset + glm::ivec3{x, y - 1, z}
                    ))
                {
                    l = 0.75f;

                    if (TerrainRenderer::DO_AMBIENT_OCCLUSION) {
                        a = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x + 1, y - 1, z}
                            ) *
                            ao_factor;
                        b = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x, y - 1, z + 1}
                            ) *
                            ao_factor;
                        c = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x - 1, y - 1, z}
                            ) *
                            ao_factor;
                        d = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x, y - 1, z - 1}
                            ) *
                            ao_factor;

                        e = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x - 1, y - 1, z - 1}
                            ) *
                            ao_factor;
                        f = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x - 1, y - 1, z + 1}
                            ) *
                            ao_factor;
                        g = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x + 1, y - 1, z + 1}
                            ) *
                            ao_factor;
                        h = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x + 1, y - 1, z - 1}
                            ) *
                            ao_factor;
                    }

                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b111, NEG_Y_NORMAL), bou1,
                         bov1, l * (1.0f - c - d - e), 1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b010, NEG_Y_NORMAL), bou2,
                         bov2, l * (1.0f - a - b - g), 1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b110, NEG_Y_NORMAL), bou1,
                         bov2, l * (1.0f - c - b - f), 1.0f, 0.0f, 0.0f}
                    );

                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b111, NEG_Y_NORMAL), bou1,
                         bov1, l * (1.0f - c - d - e), 1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b011, NEG_Y_NORMAL), bou2,
                         bov1, l * (1.0f - a - d - h), 1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b010, NEG_Y_NORMAL), bou2,
                         bov2, l * (1.0f - a - b - g), 1.0f, 0.0f, 0.0f}
                    );
                }

                if (!is_opaque(
                        chunks, self.data,
                        global_offset + glm::ivec3{x + 1, y, z}
                    ))
                {
                    l = 0.95f;

                    if (TerrainRenderer::DO_AMBIENT_OCCLUSION) {
                        a = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x + 1, y + 1, z}
                            ) *
                            ao_factor;
                        b = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x + 1, y, z + 1}
                            ) *
                            ao_factor;
                        c = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x + 1, y - 1, z}
                            ) *
                            ao_factor;
                        d = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x + 1, y, z - 1}
                            ) *
                            ao_factor;

                        e = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x + 1, y - 1, z - 1}
                            ) *
                            ao_factor;
                        f = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x + 1, y - 1, z + 1}
                            ) *
                            ao_factor;
                        g = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x + 1, y + 1, z + 1}
                            ) *
                            ao_factor;
                        h = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x + 1, y + 1, z - 1}
                            ) *
                            ao_factor;
                    }

                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b011, POS_X_NORMAL), ru2,
                         rv1, l * (1.0f - c - d - e), 0.0f, 0.0f, -1.0f}
                    );
                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b001, POS_X_NORMAL), ru2,
                         rv2, l * (1.0f - d - a - h), 0.0f, 0.0f, -1.0f}
                    );
                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b000, POS_X_NORMAL), ru1,
                         rv2, l * (1.0f - a - b - g), 0.0f, 0.0f, -1.0f}
                    );

                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b011, POS_X_NORMAL), ru2,
                         rv1, l * (1.0f - c - d - e), 0.0f, 0.0f, -1.0f}
                    );
                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b000, POS_X_NORMAL), ru1,
                         rv2, l * (1.0f - a - b - g), 0.0f, 0.0f, -1.0f}
                    );
                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b010, POS_X_NORMAL), ru1,
                         rv1, l * (1.0f - b - c - f), 0.0f, 0.0f, -1.0f}
                    );
                }
                if (!is_opaque(
                        chunks, self.data,
                        global_offset + glm::ivec3{x - 1, y, z}
                    ))
                {
                    l = 0.85f;

                    if (TerrainRenderer::DO_AMBIENT_OCCLUSION) {
                        a = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x - 1, y + 1, z}
                            ) *
                            ao_factor;
                        b = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x - 1, y, z + 1}
                            ) *
                            ao_factor;
                        c = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x - 1, y - 1, z}
                            ) *
                            ao_factor;
                        d = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x - 1, y, z - 1}
                            ) *
                            ao_factor;

                        e = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x - 1, y - 1, z - 1}
                            ) *
                            ao_factor;
                        f = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x - 1, y - 1, z + 1}
                            ) *
                            ao_factor;
                        g = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x - 1, y + 1, z + 1}
                            ) *
                            ao_factor;
                        h = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x - 1, y + 1, z - 1}
                            ) *
                            ao_factor;
                    }

                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b111, NEG_X_NORMAL), lu1,
                         lv1, l * (1.0f - c - d - e), 0.0f, 0.0f, 1.0f}
                    );
                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b100, NEG_X_NORMAL), lu2,
                         lv2, l * (1.0f - a - b - g), 0.0f, 0.0f, 1.0f}
                    );
                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b101, NEG_X_NORMAL), lu1,
                         lv2, l * (1.0f - d - a - h), 0.0f, 0.0f, 1.0f}
                    );

                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b111, NEG_X_NORMAL), lu1,
                         lv1, l * (1.0f - c - d - e), 0.0f, 0.0f, 1.0f}
                    );
                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b110, NEG_X_NORMAL), lu2,
                         lv1, l * (1.0f - b - c - f), 0.0f, 0.0f, 1.0f}
                    );
                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b100, NEG_X_NORMAL), lu2,
                         lv2, l * (1.0f - a - b - g), 0.0f, 0.0f, 1.0f}
                    );
                }

                if (!is_opaque(
                        chunks, self.data,
                        global_offset + glm::ivec3{x, y, z + 1}
                    ))
                {
                    l = 0.9f;

                    if (TerrainRenderer::DO_AMBIENT_OCCLUSION) {
                        a = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x, y + 1, z + 1}
                            ) *
                            ao_factor;
                        b = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x + 1, y, z + 1}
                            ) *
                            ao_factor;
                        c = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x, y - 1, z + 1}
                            ) *
                            ao_factor;
                        d = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x - 1, y, z + 1}
                            ) *
                            ao_factor;

                        e = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x - 1, y - 1, z + 1}
                            ) *
                            ao_factor;
                        f = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x + 1, y - 1, z + 1}
                            ) *
                            ao_factor;
                        g = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x + 1, y + 1, z + 1}
                            ) *
                            ao_factor;
                        h = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x - 1, y + 1, z + 1}
                            ) *
                            ao_factor;
                    }

                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b110, POS_Z_NORMAL), bau1,
                         bav1, l * (1.0f - c - d - e), 1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b000, POS_Z_NORMAL), bau2,
                         bav2, l * (1.0f - a - b - g), 1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b100, POS_Z_NORMAL), bau1,
                         bav2, l * (1.0f - a - d - h), 1.0f, 0.0f, 0.0f}
                    );

                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b110, POS_Z_NORMAL), bau1,
                         bav1, l * (1.0f - c - d - e), 1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b010, POS_Z_NORMAL), bau2,
                         bav1, l * (1.0f - b - c - f), 1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b000, POS_Z_NORMAL), bau2,
                         bav2, l * (1.0f - a - b - g), 1.0f, 0.0f, 0.0f}
                    );
                }
                if (!is_opaque(
                        chunks, self.data,
                        global_offset + glm::ivec3{x, y, z - 1}
                    ))
                {
                    l = 0.8f;

                    if (TerrainRenderer::DO_AMBIENT_OCCLUSION) {
                        a = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x, y + 1, z - 1}
                            ) *
                            ao_factor;
                        b = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x + 1, y, z - 1}
                            ) *
                            ao_factor;
                        c = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x, y - 1, z - 1}
                            ) *
                            ao_factor;
                        d = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x - 1, y, z - 1}
                            ) *
                            ao_factor;

                        e = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x - 1, y - 1, z - 1}
                            ) *
                            ao_factor;
                        f = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x + 1, y - 1, z - 1}
                            ) *
                            ao_factor;
                        g = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x + 1, y + 1, z - 1}
                            ) *
                            ao_factor;
                        h = is_opaque(
                                chunks, self.data,
                                global_offset + glm::ivec3{x - 1, y + 1, z - 1}
                            ) *
                            ao_factor;
                    }

                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b111, NEG_Z_NORMAL), fu2,
                         fv1, l * (1.0f - c - d - e), -1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b101, NEG_Z_NORMAL), fu2,
                         fv2, l * (1.0f - a - d - h), -1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b001, NEG_Z_NORMAL), fu1,
                         fv2, l * (1.0f - a - b - g), -1.0f, 0.0f, 0.0f}
                    );

                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b111, NEG_Z_NORMAL), fu2,
                         fv1, l * (1.0f - c - d - e), -1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b001, NEG_Z_NORMAL), fu1,
                         fv2, l * (1.0f - a - b - g), -1.0f, 0.0f, 0.0f}
                    );
                    add_vertex(
                        &buffer,
                        {encode_data({x, y, z}, 0b011, NEG_Z_NORMAL), fu1,
                         fv1, l * (1.0f - b - c - f), -1.0f, 0.0f, 0.0f}
                    );
                }
            }
        }
    }

    result_mesh->reload_buffer();
}

auto TerrainRenderer::make_empty_mesh() -> Mesh {
    return Mesh{
        std::vector<f32>{}, TerrainRenderer::VERTEX_ATTRIBUTE_SIZES,
        Primitive::Triangles
    };
}

}  // namespace tmine
