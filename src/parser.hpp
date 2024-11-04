#pragma once

#include <string_view>
#include <optional>

namespace tmine {

template <class T>
struct ParseResult {
    std::optional<T> value;
    std::string_view tail;
};

auto parse_string(std::string_view src, std::string_view match)
    -> ParseResult<std::string_view>;

auto parse_char(std::string_view src, char value) -> ParseResult<char>;

}  // namespace tmine
