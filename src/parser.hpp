#pragma once

#include <string_view>
#include <string>
#include <optional>
#include <glm/glm.hpp>

#include "types.hpp"

namespace tmine {

template <class T>
struct ParseResult {
    std::optional<T> value;
    std::string_view tail;

    auto ok(this ParseResult const& self) -> bool {
        return self.value.has_value();
    }

    template <class Self>
    auto get_value(this Self&& self) {
        return std::forward<Self>(self).value.value();
    }

    friend auto operator|(ParseResult&& self, ParseResult&& other)
        -> ParseResult {
        if (self.value.has_value()) {
            return std::move(self);
        } else {
            return std::move(other);
        }
    }
};

auto parse_string(std::string_view src, std::string_view match)
    -> ParseResult<std::string_view>;

auto parse_char(std::string_view src, char value) -> ParseResult<char>;

auto parse_integer(std::string_view src, u32 radix = 10) -> ParseResult<i64>;

auto parse_whitespace(std::string_view src, u32 min_count = 0)
    -> ParseResult<std::string_view>;

auto parse_newline(std::string_view src) -> ParseResult<std::string_view>;

namespace fnt {

struct Info {
    std::string face;
    std::string charset;
    u32 size;
    u32 horizontal_stretch;
    bool is_bold : 1;
    bool is_italic : 1;
    bool is_unicode : 1;
    bool is_smooth : 1;
    bool is_antialiased : 1;
    glm::ivec4 padding;
    glm::ivec2 spacing;
};

auto parse_string(std::string_view src) -> ParseResult<std::string_view>;

auto parse_info(std::string_view src) -> ParseResult<Info>;

}  // namespace fnt

}  // namespace tmine
