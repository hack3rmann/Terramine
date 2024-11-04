#include <cstring>
#include <cstdlib>
#include <cerrno>
#include "../parser.hpp"

namespace tmine {

static auto is_whitespace(char value) -> bool {
    return (9 <= value && value <= 13) || 32 == value;
}

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

auto parse_integer(std::string_view src, u32 radix) -> ParseResult<i64> {
    errno = 0;
    char* parse_end = nullptr;
    auto const value = std::strtoll(src.data(), &parse_end, radix);
    auto const parse_size = (usize) (parse_end - src.data());

    if (0 != errno || parse_size > src.size() || parse_size == 0) {
        return ParseResult<i64>{.value = std::nullopt, .tail = src};
    } else {
        src.remove_prefix(parse_size);
        return ParseResult<i64>{.value = (i64) value, .tail = src};
    }
}

auto parse_whitespace(std::string_view src, u32 min_count)
    -> ParseResult<std::string_view> {
    auto n_spaces = usize{0};

    for (auto symbol : src) {
        if (!is_whitespace(symbol)) {
            break;
        }

        n_spaces += 1;
    }

    if (n_spaces < min_count) {
        return ParseResult<std::string_view>{
            .value = std::nullopt, .tail = src
        };
    } else {
        auto match = src;
        match.remove_suffix(src.size() - n_spaces);
        src.remove_prefix(n_spaces);

        return ParseResult<std::string_view>{.value = match, .tail = src};
    }
}

auto parse_newline(std::string_view src) -> ParseResult<std::string_view> {
    return parse_string(src, "\r\n") | parse_string(src, "\n") |
           parse_string(src, "\r");
}

}  // namespace tmine
