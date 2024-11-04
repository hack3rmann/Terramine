#include <cstring>
#include "../parser.hpp"

namespace tmine {

auto parse_string(std::string_view src, std::string_view match)
    -> ParseResult<std::string_view> {
    if (src.size() < match.size() || !src.starts_with(match)) {
        return ParseResult<std::string_view>{
            .value = std::nullopt, .tail = src
        };
    } else {
        auto head = src;
        head.remove_suffix(src.size() - match.size());
        src.remove_prefix(match.size());

        return ParseResult<std::string_view>{.value = head, .tail = src};
    }
}

auto parse_char(std::string_view src, char value) -> ParseResult<char> {
    if (src.empty() || src[0] != value) {
        return ParseResult<char>{.value = std::nullopt, .tail = src};
    } else {
        src.remove_prefix(1);
        return ParseResult<char>{.value = value, .tail = src};
    }
}

}  // namespace tmine
