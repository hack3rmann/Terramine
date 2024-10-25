#include <cassert>

#include "../data.hpp"

namespace tmine {

    Image::Image(std::vector<u8> data, usize width, usize height)
    : data{std::move(data)}
    , width{width}
    , height{height} {}

}  // namespace tmine
