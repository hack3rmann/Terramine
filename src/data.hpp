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
    static auto constexpr TOP_TEXTURE_INDEX = usize{0};
    static auto constexpr BOTTOM_TEXTURE_INDEX = usize{1};
    static auto constexpr LEFT_TEXTURE_INDEX = usize{2};
    static auto constexpr RIGHT_TEXTURE_INDEX = usize{3};
    static auto constexpr FRONT_TEXTURE_INDEX = usize{4};
    static auto constexpr BACK_TEXTURE_INDEX = usize{5};
    static auto constexpr N_TEXTURES = usize{6};
    static auto constexpr META_TRANSLUCENT = BlockMeta{0b10000000};
    static auto constexpr META_VARIATION = BlockMeta{0b01111111};

    inline static auto constexpr meta_of(
        bool is_translucent, u8 variation
    ) noexcept -> BlockMeta {
        if (is_translucent) {
            return (variation & META_VARIATION) | META_TRANSLUCENT;
        } else {
            return variation & META_VARIATION;
        }
    }

    inline auto get_variation(this GameBlock const& self) noexcept -> u8 {
        return self.meta & GameBlock::META_VARIATION;
    }

    inline auto is_translucent(this GameBlock const& self) noexcept -> bool {
        return 0 != (self.meta & GameBlock::META_TRANSLUCENT);
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

struct GameBlocksData {
    std::vector<GameBlock> blocks;
    std::vector<GameBlockTextureIdentifier> textures;
};

}  // namespace tmine
