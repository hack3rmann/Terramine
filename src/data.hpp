#pragma once

#include <vector>
#include <string>
#include <span>
#include <filesystem>

#include "types.hpp"

namespace tmine {

class Image {
public:
    Image(std::vector<u8> data, usize width, usize height);
    ~Image() = default;
    Image(Image const&) = default;
    Image(Image&&) noexcept = default;
    auto operator=(this Image& self, Image const&) -> Image& = default;
    auto operator=(this Image& self, Image&&) noexcept -> Image& = default;

    inline auto get_width(this Image const& self) noexcept -> usize {
        return self.width;
    }

    inline auto get_height(this Image const& self) noexcept -> usize {
        return self.height;
    }

    inline auto get_data(this Image const& self
    ) noexcept -> std::span<u8 const> {
        return self.data;
    }

private:
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
