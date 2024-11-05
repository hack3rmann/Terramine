#pragma once

#include <string_view>
#include <string>
#include <optional>
#include <glm/glm.hpp>

#include "types.hpp"

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

    struct Common {
        u32 line_height;
        u32 base;
        u32 scale_width;
        u32 scale_height;
        u32 n_pages;
        bool is_packed;
    };

    struct PageHeader {
        std::string file;
        u32 id;
    };

    struct CharsHeader {
        u32 count;
    };

    struct CharDesc {
        u32 id;
        glm::uvec2 pos;
        glm::uvec2 size;
        glm::uvec2 offset;
        u32 horizontal_advance;
        u32 page_index;
        u32 channel;
    };

    struct KerningsHeader {
        u32 count;
    };

    struct Kerning {
        u32 first;
        u32 second;
        i32 amount;
    };

    struct Page {
        PageHeader header;
        CharsHeader chars_header;
        KerningsHeader kernings_header;
        std::vector<CharDesc> chars;
        std::vector<Kerning> kernings;
    };

    struct Font {
        Info info;
        Common common;
        std::vector<Page> pages;
    };

    auto parse_key_value_integer(std::string_view src, std::string_view key)
        -> ParseResult<i64>;

    auto parse_key_value_string(std::string_view src, std::string_view key)
        -> ParseResult<std::string_view>;

    auto parse_string(std::string_view src) -> ParseResult<std::string_view>;

    auto parse_info(std::string_view src) -> ParseResult<Info>;

    auto parse_common(std::string_view src) -> ParseResult<Common>;

    auto parse_page_header(std::string_view src) -> ParseResult<PageHeader>;

    auto parse_chars_header(std::string_view src) -> ParseResult<CharsHeader>;

    auto parse_char_desc(std::string_view src) -> ParseResult<CharDesc>;

    auto parse_kerning(std::string_view src) -> ParseResult<Kerning>;

    auto parse_page(std::string_view src) -> ParseResult<Page>;

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
        return Parser{[](std::string_view src) -> ParseResult<fnt::Info> {
            return ::tmine::fnt::parse_info(src);
        }};
    }

    inline auto fnt_common() {
        return Parser{[](std::string_view src) -> ParseResult<fnt::Common> {
            return ::tmine::fnt::parse_common(src);
        }};
    }

    inline auto fnt_page_header() {
        return Parser{[](std::string_view src) -> ParseResult<fnt::PageHeader> {
            return ::tmine::fnt::parse_page_header(src);
        }};
    }

    inline auto fnt_chars_header() {
        return Parser{
            [](std::string_view src) -> ParseResult<fnt::CharsHeader> {
                return ::tmine::fnt::parse_chars_header(src);
            }
        };
    }

    inline auto fnt_char_desc() {
        return Parser{[](std::string_view src) -> ParseResult<fnt::CharDesc> {
            return ::tmine::fnt::parse_char_desc(src);
        }};
    }

    inline auto fnt_kerning_header() {
        return (sequence("kernings") >> whitespace(1) >>
                fnt_key_value_integer("count"))
            .map([](i64 count) {
                return fnt::KerningsHeader{.count = (u32) count};
            });
    }

    inline auto fnt_kerning() {
        return Parser{[](std::string_view src) -> ParseResult<fnt::Kerning> {
            return ::tmine::fnt::parse_kerning(src);
        }};
    }

    inline auto fnt_page() {
        return Parser{[](std::string_view src) -> ParseResult<fnt::Page> {
            return ::tmine::fnt::parse_page(src);
        }};
    }

}  // namespace parser

}  // namespace tmine
