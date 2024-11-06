#pragma once

#include "data.hpp"
#include "graphics.hpp"

#include <unordered_map>

namespace tmine {

char constexpr SHADERS_PATH[] = "assets/shaders";

auto read_to_string(char const* path) -> std::string;

auto load_png(char const* path) -> Image;

auto load_shader_source(
    char const* vertex_source_path, char const* fragment_source_path
) -> ShaderSource;

auto load_shader(
    char const* vertex_source_path, char const* fragment_source_path
) -> ShaderProgram;

auto load_game_blocks(
    char const* path,
    std::unordered_map<std::string, GameBlockTextureIdentifier> const&
        texture_ids
) -> std::vector<GameBlock>;

auto load_game_block_textures(char const* path
) -> std::unordered_map<std::string, GameBlockTextureIdentifier>;

auto load_game_blocks_data(
    char const* game_blocks_path, char const* game_block_textures_path
) -> GameBlocksData;

auto load_font(char const* font_path) -> Font;

}  // namespace tmine
