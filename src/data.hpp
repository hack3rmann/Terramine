#pragma once

#include <vector>
#include <string>
#include <filesystem>

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
    static auto constexpr TOP_TEXUTE_ID = usize{0};
    static auto constexpr BOTTOM_TEXURE_ID = usize{1};
    static auto constexpr LEFT_TEXTURE_ID = usize{2};
    static auto constexpr RIGHT_TEXURE_ID = usize{3};
    static auto constexpr FRONT_TEXTUE_ID = usize{4};
    static auto constexpr BACK_TEXTURE_ID = usize{5};
    static auto constexpr N_TEXTURES = usize{6};
    static auto constexpr META_TRANSLUCENT = BlockMeta{0b10000000};
    static auto constexpr META_VERIATION = BlockMeta{0b01111111};

    inline static auto meta_of(bool is_translucent, u8 variation) noexcept -> BlockMeta {
        return variation | ((BlockMeta) is_translucent << 7);
    }

    std::string name;
    std::array<TextureId, N_TEXTURES> texture_ids;
    VoxelId voxel_id;
    BlockMeta meta;
};

struct GameBlockTextureIdentifier {
    std::string name;
    TextureId id;
};

}  // namespace tmine
