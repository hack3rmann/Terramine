#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <glm/glm.hpp>

#include "types.hpp"

namespace tmine {

struct Image {
    std::vector<u8> data;
    usize width;
    usize height;
};

struct ShaderSource {
    std::string vertex;
    std::string fragment;
    std::filesystem::path vertex_path;
    std::filesystem::path fragment_path;
};

using VoxelId = u8;
using TextureId = u8;
using BlockMeta = u8;

struct GameBlock {
    static auto constexpr TOP_TEXTURE_INDEX = usize{0};
    static auto constexpr BOTTOM_TEXTURE_INDEX = usize{1};
    static auto constexpr LEFT_TEXTURE_INDEX = usize{2};
    static auto constexpr RIGHT_TEXTURE_INDEX = usize{3};
    static auto constexpr FRONT_TEXTURE_INDEX = usize{4};
    static auto constexpr BACK_TEXTURE_INDEX = usize{5};
    static auto constexpr N_TEXTURES = usize{6};
    static auto constexpr META_TRANSLUCENT = BlockMeta{0b10000000};
    static auto constexpr META_EXTRA_TRANSPARENT = BlockMeta{0b01000000};
    static auto constexpr META_ORIENTATION = BlockMeta{0b00000111};
    static auto constexpr ORIENTATION_POS_X = BlockMeta{0};
    static auto constexpr ORIENTATION_NEG_X = BlockMeta{1};
    static auto constexpr ORIENTATION_POS_Y = BlockMeta{2};
    static auto constexpr ORIENTATION_NEG_Y = BlockMeta{3};
    static auto constexpr ORIENTATION_POS_Z = BlockMeta{4};
    static auto constexpr ORIENTATION_NEG_Z = BlockMeta{5};

    std::string name;
    std::array<TextureId, N_TEXTURES> texture_ids;
    VoxelId voxel_id;
    BlockMeta meta;

    inline static auto constexpr meta_of(
        bool is_translucent, bool is_extra_transparent, u8 orientation
    ) noexcept -> BlockMeta {
        return (orientation & META_ORIENTATION) |
               (BlockMeta{is_translucent} << 7) |
               (BlockMeta{is_extra_transparent} << 6);
    }

    inline auto get_orientation(this GameBlock const& self) noexcept -> u8 {
        return self.meta & GameBlock::META_ORIENTATION;
    }

    inline auto is_translucent(this GameBlock const& self) noexcept -> bool {
        return 0 != (self.meta & GameBlock::META_TRANSLUCENT);
    }

    inline auto is_extra_transparent(this GameBlock const& self) noexcept
        -> bool {
        return 0 != (self.meta & GameBlock::META_EXTRA_TRANSPARENT);
    }
};

struct GameBlockTextureIdentifier {
    std::string name;
    TextureId id;
};

struct Side {
    using Value = u32;

    static auto constexpr EMPTY = Value{0};
    static auto constexpr POS_X = Value{1 << 0};
    static auto constexpr NEG_X = Value{1 << 1};
    static auto constexpr POS_Y = Value{1 << 2};
    static auto constexpr NEG_Y = Value{1 << 3};
    static auto constexpr POS_Z = Value{1 << 4};
    static auto constexpr NEG_Z = Value{1 << 5};
    static auto constexpr ALL = POS_X | NEG_X | POS_Y | NEG_Y | POS_Z | NEG_Z;

    static auto constexpr contains(Value self, Value flags) -> bool {
        return 0 != (self & flags);
    }
};

using SideFlags = Side::Value;

enum class Orientation {
    PosX = 0,
    NegX = 1,
    PosY = 2,
    NegY = 3,
    PosZ = 4,
    NegZ = 5,
};

struct GameBlocksData {
    static auto constexpr POS_X = (usize) Orientation::PosX;
    static auto constexpr NEG_X = (usize) Orientation::NegX;
    static auto constexpr POS_Y = (usize) Orientation::PosY;
    static auto constexpr NEG_Y = (usize) Orientation::NegY;
    static auto constexpr POS_Z = (usize) Orientation::PosZ;
    static auto constexpr NEG_Z = (usize) Orientation::NegZ;
    static auto constexpr N_ORIENTATIONS = usize{6};

    std::vector<std::array<GameBlock, N_ORIENTATIONS>> blocks;
    std::vector<GameBlockTextureIdentifier> textures;

    auto get_block(this GameBlocksData const& self, VoxelId id, Orientation orientation)
        -> GameBlock const& {
        return self.blocks[(usize) id][(usize) orientation];
    }
};

struct FontInfo {
    std::string face;
    u32 size;
    bool is_bold : 4;
    bool is_italic : 4;
    std::string charset;
    bool is_unicode;
    u32 horizontal_stretch;
    bool is_smooth : 4;
    bool is_antialiased : 4;
    glm::ivec4 padding;
    glm::ivec2 spacing;
};

struct FontCommon {
    u32 line_height;
    u32 base;
    glm::uvec2 scale;
    u32 n_pages;
    bool is_packed;
};

struct FontPageHeader {
    u32 id;
    std::string file;
};

struct FontCharsHeader {
    u32 count;
};

struct FontCharDesc {
    u32 id;
    glm::uvec2 pos;
    glm::uvec2 size;
    glm::ivec2 offset;
    u32 horizontal_advance;
    u32 page_index;
    u32 channel;
};

struct FontKerningsHeader {
    u32 count;
};

struct FontKerning {
    u32 first;
    u32 second;
    i32 amount;
};

struct FontPage {
    FontPageHeader header;
    FontCharsHeader chars_header;
    std::vector<FontCharDesc> chars;
    FontKerningsHeader kernings_header;
    std::vector<FontKerning> kernings;
};

struct Font {
    FontInfo info;
    FontCommon common;
    std::vector<FontPage> pages;
};

}  // namespace tmine
