#include "../terrain.hpp"

namespace tmine {

auto constexpr POS_X_NORMAL = u32{0};
auto constexpr NEG_X_NORMAL = u32{1};
auto constexpr POS_Y_NORMAL = u32{2};
auto constexpr NEG_Y_NORMAL = u32{3};
auto constexpr POS_Z_NORMAL = u32{4};
auto constexpr NEG_Z_NORMAL = u32{5};

static auto encode_opaque(
    glm::uvec3 pos, u32 offset, u32 normal, f32 light, u32 texture_id,
    u32 corner_index
) -> f32 {
    static_assert(
        Chunk::N_POSITION_BITS == 4, "only 4 bit position is supported"
    );

    auto constexpr TEXTURE_ATLAS_SIZE = u32{16};

    auto compressed_light = 15u & u8(light * 15.0f);
    auto texture_coords = glm::uvec2{
        texture_id / TEXTURE_ATLAS_SIZE, texture_id % TEXTURE_ATLAS_SIZE
    };

    auto constexpr N_OFFSET_BITS = 3;
    auto constexpr N_NORMAL_BITS = 3;
    auto constexpr N_TEXTURE_COORD_BITS = 4;
    auto constexpr N_LIGHT_BITS = 4;
    auto result = u32{0};

    result |= pos.x << (0 * Chunk::N_POSITION_BITS);
    result |= pos.y << (1 * Chunk::N_POSITION_BITS);
    result |= pos.z << (2 * Chunk::N_POSITION_BITS);
    result |= offset << (3 * Chunk::N_POSITION_BITS);
    result |= normal << (3 * Chunk::N_POSITION_BITS + N_OFFSET_BITS);
    result |= (u32) compressed_light
           << (3 * Chunk::N_POSITION_BITS + N_OFFSET_BITS + N_NORMAL_BITS);
    result |= texture_coords.x
           << (3 * Chunk::N_POSITION_BITS + N_OFFSET_BITS + N_NORMAL_BITS +
               N_LIGHT_BITS + 0 * N_TEXTURE_COORD_BITS);
    result |= texture_coords.y
           << (3 * Chunk::N_POSITION_BITS + N_OFFSET_BITS + N_NORMAL_BITS +
               N_LIGHT_BITS + 1 * N_TEXTURE_COORD_BITS);
    result |= corner_index
           << (3 * Chunk::N_POSITION_BITS + N_OFFSET_BITS + N_NORMAL_BITS +
               N_LIGHT_BITS + 2 * N_TEXTURE_COORD_BITS);

    return std::bit_cast<f32>(result);
}

static auto encode_transparent(
    glm::ivec3 global_offset, glm::vec3 pos, u32 offset_bits, u32 normal, u32 texture_id, u32 corner_index
) -> TerrainRenderer::TransparentVertex {
    auto constexpr TEXTURE_ATLAS_SIZE = u32{16};

    auto texture_coords = glm::uvec2{
        texture_id / TEXTURE_ATLAS_SIZE, texture_id % TEXTURE_ATLAS_SIZE
    };

    auto offset = glm::vec3(
        1.0f - (f32) (1u & (offset_bits >> 2u)),
        1.0f - (f32) (1u & (offset_bits >> 1u)),
        1.0f - (f32) (1u & (offset_bits >> 0u))
    );

    auto constexpr N_NORMAL_BITS = 3;
    auto constexpr N_TEXTURE_COORD_BITS = 4;
    auto result = u32{0};

    result |= normal << 0;
    result |= texture_coords.x << (N_NORMAL_BITS + 0 * N_TEXTURE_COORD_BITS);
    result |= texture_coords.y << (N_NORMAL_BITS + 1 * N_TEXTURE_COORD_BITS);
    result |= corner_index << (N_NORMAL_BITS + 2 * N_TEXTURE_COORD_BITS);

    return TerrainRenderer::TransparentVertex{
        .pos = pos + glm::vec3{global_offset} + offset,
        .data = std::bit_cast<f32>(result),
    };
}

static auto get_vertices(
    glm::ivec3 global_offset,
    glm::vec3 pos,
    std::array<TextureId, 6> texture_ids
) -> std::array<TerrainRenderer::TransparentVertex, 36> {
    using V = TerrainRenderer::TransparentVertex;

    auto x = pos.x;
    auto y = pos.y;
    auto z = pos.z;

    auto constexpr TOP = GameBlock::TOP_TEXTURE_INDEX;
    auto constexpr BOTTOM = GameBlock::BOTTOM_TEXTURE_INDEX;
    auto constexpr RIGHT = GameBlock::RIGHT_TEXTURE_INDEX;
    auto constexpr LEFT = GameBlock::LEFT_TEXTURE_INDEX;
    auto constexpr BACK = GameBlock::BACK_TEXTURE_INDEX;
    auto constexpr FRONT = GameBlock::FRONT_TEXTURE_INDEX;

    auto constexpr encode = encode_transparent;

    return {
        V{encode(global_offset, {x, y, z}, 0b101, POS_Y_NORMAL, texture_ids[TOP], 0b10)},
        V{encode(global_offset, {x, y, z}, 0b100, POS_Y_NORMAL, texture_ids[TOP], 0b11)},
        V{encode(global_offset, {x, y, z}, 0b000, POS_Y_NORMAL, texture_ids[TOP], 0b01)},
        V{encode(global_offset, {x, y, z}, 0b101, POS_Y_NORMAL, texture_ids[TOP], 0b10)},
        V{encode(global_offset, {x, y, z}, 0b000, POS_Y_NORMAL, texture_ids[TOP], 0b01)},
        V{encode(global_offset, {x, y, z}, 0b001, POS_Y_NORMAL, texture_ids[TOP], 0b00)},

        V{encode(global_offset, {x, y, z}, 0b111, NEG_Y_NORMAL, texture_ids[BOTTOM], 0b00)},
        V{encode(global_offset, {x, y, z}, 0b010, NEG_Y_NORMAL, texture_ids[BOTTOM], 0b11)},
        V{encode(global_offset, {x, y, z}, 0b110, NEG_Y_NORMAL, texture_ids[BOTTOM], 0b01)},
        V{encode(global_offset, {x, y, z}, 0b111, NEG_Y_NORMAL, texture_ids[BOTTOM], 0b00)},
        V{encode(global_offset, {x, y, z}, 0b011, NEG_Y_NORMAL, texture_ids[BOTTOM], 0b10)},
        V{encode(global_offset, {x, y, z}, 0b010, NEG_Y_NORMAL, texture_ids[BOTTOM], 0b11)},

        V{encode(global_offset, {x, y, z}, 0b011, POS_X_NORMAL, texture_ids[RIGHT], 0b10)},
        V{encode(global_offset, {x, y, z}, 0b001, POS_X_NORMAL, texture_ids[RIGHT], 0b11)},
        V{encode(global_offset, {x, y, z}, 0b000, POS_X_NORMAL, texture_ids[RIGHT], 0b01)},
        V{encode(global_offset, {x, y, z}, 0b011, POS_X_NORMAL, texture_ids[RIGHT], 0b10)},
        V{encode(global_offset, {x, y, z}, 0b000, POS_X_NORMAL, texture_ids[RIGHT], 0b01)},
        V{encode(global_offset, {x, y, z}, 0b010, POS_X_NORMAL, texture_ids[RIGHT], 0b00)},

        V{encode(global_offset, {x, y, z}, 0b111, NEG_X_NORMAL, texture_ids[LEFT], 0b00)},
        V{encode(global_offset, {x, y, z}, 0b100, NEG_X_NORMAL, texture_ids[LEFT], 0b11)},
        V{encode(global_offset, {x, y, z}, 0b101, NEG_X_NORMAL, texture_ids[LEFT], 0b01)},
        V{encode(global_offset, {x, y, z}, 0b111, NEG_X_NORMAL, texture_ids[LEFT], 0b00)},
        V{encode(global_offset, {x, y, z}, 0b110, NEG_X_NORMAL, texture_ids[LEFT], 0b10)},
        V{encode(global_offset, {x, y, z}, 0b100, NEG_X_NORMAL, texture_ids[LEFT], 0b11)},

        V{encode(global_offset, {x, y, z}, 0b110, POS_Z_NORMAL, texture_ids[BACK], 0b00)},
        V{encode(global_offset, {x, y, z}, 0b000, POS_Z_NORMAL, texture_ids[BACK], 0b11)},
        V{encode(global_offset, {x, y, z}, 0b100, POS_Z_NORMAL, texture_ids[BACK], 0b01)},
        V{encode(global_offset, {x, y, z}, 0b110, POS_Z_NORMAL, texture_ids[BACK], 0b00)},
        V{encode(global_offset, {x, y, z}, 0b010, POS_Z_NORMAL, texture_ids[BACK], 0b10)},
        V{encode(global_offset, {x, y, z}, 0b000, POS_Z_NORMAL, texture_ids[BACK], 0b11)},

        V{encode(global_offset, {x, y, z}, 0b111, NEG_Z_NORMAL, texture_ids[FRONT], 0b10)},
        V{encode(global_offset, {x, y, z}, 0b101, NEG_Z_NORMAL, texture_ids[FRONT], 0b11)},
        V{encode(global_offset, {x, y, z}, 0b001, NEG_Z_NORMAL, texture_ids[FRONT], 0b01)},
        V{encode(global_offset, {x, y, z}, 0b111, NEG_Z_NORMAL, texture_ids[FRONT], 0b10)},
        V{encode(global_offset, {x, y, z}, 0b001, NEG_Z_NORMAL, texture_ids[FRONT], 0b01)},
        V{encode(global_offset, {x, y, z}, 0b011, NEG_Z_NORMAL, texture_ids[FRONT], 0b00)},
    };
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
    RefMut<Mesh<TerrainRenderer::Vertex>> result_mesh,
    RefMut<TransparentMesh> transparent_mesh,
    TerrainRenderUploadMesh upload
) -> void {
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

                auto const top_texture_id =
                    data.texture_ids[GameBlock::TOP_TEXTURE_INDEX];
                auto const bottom_texture_id =
                    data.texture_ids[GameBlock::BOTTOM_TEXTURE_INDEX];
                auto const left_texture_id =
                    data.texture_ids[GameBlock::LEFT_TEXTURE_INDEX];
                auto const right_texture_id =
                    data.texture_ids[GameBlock::RIGHT_TEXTURE_INDEX];
                auto const front_texture_id =
                    data.texture_ids[GameBlock::FRONT_TEXTURE_INDEX];
                auto const back_texture_id =
                    data.texture_ids[GameBlock::BACK_TEXTURE_INDEX];

                if (data.is_translucent()) {
                    auto& buffer = transparent_mesh->get_buffer();

                    auto vertices = get_vertices(
                        global_offset, {x, y, z}, data.texture_ids
                    );

                    buffer.append(vertices);

                    continue;
                }

                f32 l = 1.0f;

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

                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b101, POS_Y_NORMAL, l * (1.0f - c - d - e),
                        top_texture_id, 0b10
                    ));
                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b100, POS_Y_NORMAL, l * (1.0f - c - b - f),
                        top_texture_id, 0b11
                    ));
                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b000, POS_Y_NORMAL, l * (1.0f - a - b - g),
                        top_texture_id, 0b01
                    ));

                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b101, POS_Y_NORMAL, l * (1.0f - c - d - e),
                        top_texture_id, 0b10
                    ));
                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b000, POS_Y_NORMAL, l * (1.0f - a - b - g),
                        top_texture_id, 0b01
                    ));
                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b001, POS_Y_NORMAL, l * (1.0f - a - d - h),
                        top_texture_id, 0b00
                    ));
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

                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b111, NEG_Y_NORMAL, l * (1.0f - c - d - e),
                        bottom_texture_id, 0b00
                    ));
                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b010, NEG_Y_NORMAL, l * (1.0f - a - b - g),
                        bottom_texture_id, 0b11
                    ));
                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b110, NEG_Y_NORMAL, l * (1.0f - c - b - f),
                        bottom_texture_id, 0b01
                    ));

                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b111, NEG_Y_NORMAL, l * (1.0f - c - d - e),
                        bottom_texture_id, 0b00
                    ));
                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b011, NEG_Y_NORMAL, l * (1.0f - a - d - h),
                        bottom_texture_id, 0b10
                    ));
                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b010, NEG_Y_NORMAL, l * (1.0f - a - b - g),
                        bottom_texture_id, 0b11
                    ));
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

                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b011, POS_X_NORMAL, l * (1.0f - c - d - e),
                        right_texture_id, 0b10
                    ));
                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b001, POS_X_NORMAL, l * (1.0f - d - a - h),
                        right_texture_id, 0b11
                    ));
                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b000, POS_X_NORMAL, l * (1.0f - a - b - g),
                        right_texture_id, 0b01
                    ));

                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b011, POS_X_NORMAL, l * (1.0f - c - d - e),
                        right_texture_id, 0b10
                    ));
                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b000, POS_X_NORMAL, l * (1.0f - a - b - g),
                        right_texture_id, 0b01
                    ));
                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b010, POS_X_NORMAL, l * (1.0f - b - c - f),
                        right_texture_id, 0b00
                    ));
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

                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b111, NEG_X_NORMAL, l * (1.0f - c - d - e),
                        left_texture_id, 0b00
                    ));
                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b100, NEG_X_NORMAL, l * (1.0f - a - b - g),
                        left_texture_id, 0b11
                    ));
                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b101, NEG_X_NORMAL, l * (1.0f - d - a - h),
                        left_texture_id, 0b01
                    ));

                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b111, NEG_X_NORMAL, l * (1.0f - c - d - e),
                        left_texture_id, 0b00
                    ));
                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b110, NEG_X_NORMAL, l * (1.0f - b - c - f),
                        left_texture_id, 0b10
                    ));
                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b100, NEG_X_NORMAL, l * (1.0f - a - b - g),
                        left_texture_id, 0b11
                    ));
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

                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b110, POS_Z_NORMAL, l * (1.0f - c - d - e),
                        back_texture_id, 0b00
                    ));
                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b000, POS_Z_NORMAL, l * (1.0f - a - b - g),
                        back_texture_id, 0b11
                    ));
                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b100, POS_Z_NORMAL, l * (1.0f - a - d - h),
                        back_texture_id, 0b01
                    ));

                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b110, POS_Z_NORMAL, l * (1.0f - c - d - e),
                        back_texture_id, 0b00
                    ));
                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b010, POS_Z_NORMAL, l * (1.0f - b - c - f),
                        back_texture_id, 0b10
                    ));
                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b000, POS_Z_NORMAL, l * (1.0f - a - b - g),
                        back_texture_id, 0b11
                    ));
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

                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b111, NEG_Z_NORMAL, l * (1.0f - c - d - e),
                        front_texture_id, 0b10
                    ));
                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b101, NEG_Z_NORMAL, l * (1.0f - a - d - h),
                        front_texture_id, 0b11
                    ));
                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b001, NEG_Z_NORMAL, l * (1.0f - a - b - g),
                        front_texture_id, 0b01
                    ));

                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b111, NEG_Z_NORMAL, l * (1.0f - c - d - e),
                        front_texture_id, 0b10
                    ));
                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b001, NEG_Z_NORMAL, l * (1.0f - a - b - g),
                        front_texture_id, 0b01
                    ));
                    buffer.emplace_back(encode_opaque(
                        {x, y, z}, 0b011, NEG_Z_NORMAL, l * (1.0f - b - c - f),
                        front_texture_id, 0b00
                    ));
                }
            }
        }
    }

    if (TerrainRenderUploadMesh::DoUpload == upload) {
        result_mesh->reload_buffer();
        transparent_mesh->reload_buffer();
    }
}

}  // namespace tmine
