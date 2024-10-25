#pragma once

#include "data.hpp"

#include <filesystem>
#include <optional>

namespace tmine {

char constexpr SHADERS_PATH[] = "assets/Shaders";

auto read_to_string(std::filesystem::path path) -> std::optional<std::string>;

auto load_png(std::filesystem::path image_path) -> std::optional<Image>;

auto load_shader(
    std::filesystem::path vertex_source_path,
    std::filesystem::path fragment_source_path
) -> std::optional<ShaderSource>;

}  // namespace tmine
