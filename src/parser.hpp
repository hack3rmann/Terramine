#pragma once

#include <string_view>
#include <glm/glm.hpp>
#include <comb/parse.hpp>

#include "types.hpp"
#include "data.hpp"

namespace tmine {

namespace fnt {

    auto parse_key_value_integer(std::string_view src, std::string_view key)
        -> comb::ParseResult<i64>;

    auto parse_key_value_string(std::string_view src, std::string_view key)
        -> comb::ParseResult<std::string_view>;

    auto parse_info(std::string_view src) -> comb::ParseResult<FontInfo>;

    auto parse_common(std::string_view src) -> comb::ParseResult<FontCommon>;

    auto parse_page_header(std::string_view src)
        -> comb::ParseResult<FontPageHeader>;

    auto parse_chars_header(std::string_view src)
        -> comb::ParseResult<FontCharsHeader>;

    auto parse_char_desc(std::string_view src)
        -> comb::ParseResult<FontCharDesc>;

    auto parse_kerning(std::string_view src) -> comb::ParseResult<FontKerning>;

    auto parse_page(std::string_view src) -> comb::ParseResult<FontPage>;

    auto parse_font(std::string_view src) -> comb::ParseResult<Font>;

}  // namespace fnt

namespace parser {

    inline auto fnt_key_value_string(std::string_view key) {
        return comb::Parser{
            [key](std::string_view src) -> comb::ParseResult<std::string_view> {
                return ::tmine::fnt::parse_key_value_string(src, key);
            }
        };
    }

    inline auto fnt_key_value_integer(std::string_view key) {
        return comb::Parser{
            [key](std::string_view src) -> comb::ParseResult<i64> {
                return ::tmine::fnt::parse_key_value_integer(src, key);
            }
        };
    }

    inline auto fnt_info() {
        return comb::Parser{
            [](std::string_view src) -> comb::ParseResult<FontInfo> {
                return ::tmine::fnt::parse_info(src);
            }
        };
    }

    inline auto fnt_common() {
        return comb::Parser{
            [](std::string_view src) -> comb::ParseResult<FontCommon> {
                return ::tmine::fnt::parse_common(src);
            }
        };
    }

    inline auto fnt_page_header() {
        return comb::Parser{
            [](std::string_view src) -> comb::ParseResult<FontPageHeader> {
                return ::tmine::fnt::parse_page_header(src);
            }
        };
    }

    inline auto fnt_chars_header() {
        return comb::Parser{
            [](std::string_view src) -> comb::ParseResult<FontCharsHeader> {
                return ::tmine::fnt::parse_chars_header(src);
            }
        };
    }

    inline auto fnt_char_desc() {
        return comb::Parser{
            [](std::string_view src) -> comb::ParseResult<FontCharDesc> {
                return ::tmine::fnt::parse_char_desc(src);
            }
        };
    }

    inline auto fnt_kerning_header() {
        return (comb::prefix("kernings") >> comb::whitespace(1) >>
                fnt_key_value_integer("count"))
            .map([](i64 count) {
                return FontKerningsHeader{.count = (u32) count};
            });
    }

    inline auto fnt_kerning() {
        return comb::Parser{
            [](std::string_view src) -> comb::ParseResult<FontKerning> {
                return ::tmine::fnt::parse_kerning(src);
            }
        };
    }

    inline auto fnt_page() {
        return comb::Parser{
            [](std::string_view src) -> comb::ParseResult<FontPage> {
                return ::tmine::fnt::parse_page(src);
            }
        };
    }

}  // namespace parser

}  // namespace tmine
