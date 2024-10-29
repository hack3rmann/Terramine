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

}  // namespace tmine
