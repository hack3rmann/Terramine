#pragma once

#include <string_view>
#include <optional>
#include <glm/glm.hpp>

#include "types.hpp"
#include "data.hpp"

#define parse_unwrap(tail, call)                          \
    ({                                                    \
        auto const result = (call);                       \
        tail = result.tail;                               \
                                                          \
        if (!result.ok()) {                               \
            return {.value = std::nullopt, .tail = tail}; \
        }                                                 \
                                                          \
        result.get_value();                               \
    })

#define execute_parser(tail, parser) parse_unwrap(tail, (parser) (tail))

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

    auto parse_key_value_integer(std::string_view src, std::string_view key)
        -> ParseResult<i64>;

    auto parse_key_value_string(std::string_view src, std::string_view key)
        -> ParseResult<std::string_view>;

    auto parse_string(std::string_view src) -> ParseResult<std::string_view>;

    auto parse_info(std::string_view src) -> ParseResult<FontInfo>;

    auto parse_common(std::string_view src) -> ParseResult<FontCommon>;

    auto parse_page_header(std::string_view src) -> ParseResult<FontPageHeader>;

    auto parse_chars_header(std::string_view src) -> ParseResult<FontCharsHeader>;

    auto parse_char_desc(std::string_view src) -> ParseResult<FontCharDesc>;

    auto parse_kerning(std::string_view src) -> ParseResult<FontKerning>;

    auto parse_page(std::string_view src) -> ParseResult<FontPage>;

    auto parse_font(std::string_view src) -> ParseResult<Font>;

}  // namespace fnt

namespace parser {

    template <class T>
    struct Parser {
        T parse;

        template <class Self>
        auto operator()(this Self&& self, std::string_view src) {
            return std::forward<Self>(self).parse(src);
        }

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
        friend auto operator<<(Parser<T>&& lhs, Parser<S>&& rhs) {
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

        template <class F>
        auto map(this Parser&& self, F transform) {
            auto parse = [=](std::string_view src) {
                auto result = (self) (src);

                using NewType = decltype(transform(result.get_value()));

                if (result.ok()) {
                    return ParseResult<NewType>{
                        .value = transform(std::move(result.get_value())),
                        .tail = result.tail
                    };
                } else {
                    return ParseResult<NewType>{
                        .value = std::nullopt, .tail = result.tail
                    };
                }
            };

            return Parser<decltype(parse)>{std::move(parse)};
        }

        auto sequence(this Parser&& self, usize min_count = 0) {
            auto parse = [=](std::string_view src) {
                using Value = decltype(self.parse(src).get_value());
                using Sequence = std::vector<Value>;

                auto result_sequence = std::vector<Value>{};
                auto tail = src;

                for (auto result = self.parse(tail); result.ok();
                     result = self.parse(tail))
                {
                    result_sequence.emplace_back(std::move(result).get_value());
                    tail = result.tail;
                }

                if (result_sequence.size() < min_count) {
                    return ParseResult<Sequence>{
                        .value = std::nullopt,
                        .tail = src,
                    };
                } else {
                    return ParseResult<Sequence>{
                        .value = std::move(result_sequence),
                        .tail = tail,
                    };
                }
            };

            return Parser<decltype(parse)>(std::move(parse));
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
        return Parser{
            [min_count](std::string_view src) -> ParseResult<std::string_view> {
                return parse_whitespace(src, min_count);
            }
        };
    }

    inline auto newline() {
        return Parser{
            [](std::string_view src) -> ParseResult<std::string_view> {
                return parse_newline(src);
            }
        };
    }

    inline auto quoted_string() {
        return Parser{
            [](std::string_view src) -> ParseResult<std::string_view> {
                return ::tmine::fnt::parse_string(src);
            }
        };
    }

    inline auto fnt_key_value_string(std::string_view key) {
        return Parser{
            [key](std::string_view src) -> ParseResult<std::string_view> {
                return ::tmine::fnt::parse_key_value_string(src, key);
            }
        };
    }

    inline auto fnt_key_value_integer(std::string_view key) {
        return Parser{[key](std::string_view src) -> ParseResult<i64> {
            return ::tmine::fnt::parse_key_value_integer(src, key);
        }};
    }

    inline auto fnt_info() {
        return Parser{[](std::string_view src) -> ParseResult<FontInfo> {
            return ::tmine::fnt::parse_info(src);
        }};
    }

    inline auto fnt_common() {
        return Parser{[](std::string_view src) -> ParseResult<FontCommon> {
            return ::tmine::fnt::parse_common(src);
        }};
    }

    inline auto fnt_page_header() {
        return Parser{[](std::string_view src) -> ParseResult<FontPageHeader> {
            return ::tmine::fnt::parse_page_header(src);
        }};
    }

    inline auto fnt_chars_header() {
        return Parser{
            [](std::string_view src) -> ParseResult<FontCharsHeader> {
                return ::tmine::fnt::parse_chars_header(src);
            }
        };
    }

    inline auto fnt_char_desc() {
        return Parser{[](std::string_view src) -> ParseResult<FontCharDesc> {
            return ::tmine::fnt::parse_char_desc(src);
        }};
    }

    inline auto fnt_kerning_header() {
        return (sequence("kernings") >> whitespace(1) >>
                fnt_key_value_integer("count"))
            .map([](i64 count) {
                return FontKerningsHeader{.count = (u32) count};
            });
    }

    inline auto fnt_kerning() {
        return Parser{[](std::string_view src) -> ParseResult<FontKerning> {
            return ::tmine::fnt::parse_kerning(src);
        }};
    }

    inline auto fnt_page() {
        return Parser{[](std::string_view src) -> ParseResult<FontPage> {
            return ::tmine::fnt::parse_page(src);
        }};
    }

}  // namespace parser

}  // namespace tmine
