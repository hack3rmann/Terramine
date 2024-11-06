#include <fmt/format.h>
#include "../loaders.hpp"
#include "../parser.hpp"

namespace tmine {

using namespace fnt;
using namespace parser;

auto load_font(char const* path) -> Font {
    auto const font_text = read_to_string(path);
    auto const parse_result = parse_font(font_text);

    if (!parse_result.ok()) {
        throw std::runtime_error(fmt::format("failed to parse font {}", path));
    }

    return std::move(parse_result).get_value();
}

}  // namespace tmine
