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

    friend auto operator|(ParseResult&& lhs, ParseResult&& rhs) -> ParseResult {
        if (lhs.ok()) {
            return std::move(lhs);
        } else {
            return std::move(rhs);
        }
    }
};

auto parse_sequence(std::string_view src, std::string_view match)
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

auto parse_key_value_integer(std::string_view src, std::string_view key)
    -> ParseResult<i64>;

auto parse_key_value_string(std::string_view src, std::string_view key)
    -> ParseResult<std::string_view>;

auto parse_string(std::string_view src) -> ParseResult<std::string_view>;

auto parse_info(std::string_view src) -> ParseResult<Info>;

}  // namespace fnt

namespace parser {

template <class T>
struct Parser {
    T parse;

    template <class S>
    friend auto operator|(Parser<T>&& lhs, Parser<S>&& rhs) {
        auto parse = [=](std::string_view src) {
            auto left_result = std::move(lhs.parse)(src);

            if (left_result.ok()) {
                return std::move(left_result);
            } else {
                auto right_result = std::move(rhs.parse)(src);
                return std::move(right_result);
            }
        };

        return Parser<decltype(parse)>{std::move(parse)};
    }

    template <class S>
    friend auto operator>>(Parser<T>&& lhs, Parser<S>&& rhs) {
        auto parse = [=](std::string_view src) {
            auto left_result = std::move(lhs.parse)(src);

            if (!left_result.ok()) {
                return decltype(std::move(rhs.parse)(src)
                ){.value = ::std::nullopt, .tail = src};
            } else {
                auto right_result = std::move(rhs.parse)(left_result.tail);
                return std::move(right_result);
            }
        };

        return Parser<decltype(parse)>{std::move(parse)};
    }

    template <class S>
    friend auto operator<<(Parser<T>&& lhs, Parser<S> const& rhs) {
        auto parse = [=](std::string_view src) {
            auto left_result = std::move(lhs.parse)(src);

            if (left_result.ok()) {
                auto right_result = std::move(rhs.parse)(left_result.tail);

                if (right_result.ok()) {
                    return decltype(left_result
                    ){.value = std::move(left_result).value,
                      .tail = right_result.tail};
                } else {
                    return decltype(left_result
                    ){.value = ::std::nullopt, .tail = src};
                }
            } else {
                return left_result;
            }
        };

        return Parser<decltype(parse)>{std::move(parse)};
    }
};

inline auto sequence(std::string_view match) {
    return Parser{
        [match](std::string_view src) -> ParseResult<std::string_view> {
            return parse_sequence(src, match);
        }
    };
}

inline auto character(char value) {
    return Parser{[value](std::string_view src) -> ParseResult<char> {
        return parse_char(src, value);
    }};
}

inline auto integer(u32 radix = 10) {
    return Parser{[radix](std::string_view src) -> ParseResult<i64> {
        return parse_integer(src, radix);
    }};
}

inline auto whitespace(u32 min_count = 0) {
    return Parser{[min_count](std::string_view src) -> ParseResult<std::string_view> {
        return parse_whitespace(src, min_count);
    }};
}

inline auto newline() {
    return Parser{[](std::string_view src) -> ParseResult<std::string_view> {
        return parse_newline(src);
    }};
}

inline auto quoted_string() {
    return Parser{[](std::string_view src) -> ParseResult<std::string_view> {
        return ::tmine::fnt::parse_string(src);
    }};
}

}  // namespace parser

}  // namespace tmine
