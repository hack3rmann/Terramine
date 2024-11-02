#pragma once

#include "data.hpp"
#include "graphics.hpp"

#include <optional>
#include <unordered_map>

namespace tmine {

char constexpr SHADERS_PATH[] = "assets/Shaders";

auto read_to_string(char const* path) -> std::optional<std::string>;

auto load_png(char const* path) -> std::optional<Image>;

auto load_shader_source(
    char const* vertex_source_path, char const* fragment_source_path
) -> std::optional<ShaderSource>;

auto load_shader(
    char const* vertex_source_path, char const* fragment_source_path
) -> std::optional<ShaderProgram>;

auto load_game_blocks(
    char const* path,
    std::unordered_map<std::string, GameBlockTextureIdentifier> const&
        texture_ids
) -> std::optional<std::vector<GameBlock>>;

auto load_game_block_textures(char const* path
) -> std::optional<std::unordered_map<std::string, GameBlockTextureIdentifier>>;

auto load_game_blocks_data(
    char const* game_blocks_path, char const* game_block_textures_path
) -> std::optional<GameBlocksData>;

}  // namespace tmine
