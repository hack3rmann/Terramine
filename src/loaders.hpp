#pragma once

#include "data.hpp"

#include <filesystem>
#include <optional>

namespace tmine {

    auto load_png(std::filesystem::path image_path) -> std::optional<Image>;

}  // namespace tmine
