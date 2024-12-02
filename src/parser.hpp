#pragma once

#include <string_view>
#include <glm/glm.hpp>
#include <comb/parse.hpp>

#include "data.hpp"

namespace tmine {

namespace parser {

    using namespace comb;

    inline auto constexpr key_value(std::string_view key, ParserLike auto value)
        -> ParserLike auto {
        return prefix(key) >> character('=') >> std::move(value);
    }

    auto constexpr parse_info =
        prefix("info") >> whitespace(1) >>
        collect<FontInfo>(
            key_value("face", quoted_string()).map_type<std::string>()
                << whitespace(1),
            key_value("size", integer()) << whitespace(1),
            key_value("bold", integer()).map_type<bool>() << whitespace(1),
            key_value("italic", integer()).map_type<bool>() << whitespace(1),
            key_value("charset", quoted_string()).map_type<std::string>()
                << whitespace(1),
            key_value("unicode", integer()).map_type<bool>() << whitespace(1),
            key_value("stretchH", integer()) << whitespace(1),
            key_value("smooth", integer()).map_type<bool>() << whitespace(1),
            key_value("aa", integer()).map_type<bool>() << whitespace(1),
            key_value(
                "padding",
                collect<glm::ivec4>(
                    integer() << character(','), integer() << character(','),
                    integer() << character(','), integer()
                )
            ) << whitespace(1),
            key_value(
                "spacing",
                collect<glm::ivec2>(integer() << character(','), integer())
            )
        );

    auto constexpr parse_common =
        prefix("common") >>
        whitespace(1) >> collect<FontCommon>(
                             key_value("lineHeight", integer())
                                 << whitespace(1),
                             key_value("base", integer()) << whitespace(1),
                             collect<glm::uvec2>(
                                 key_value("scaleW", integer())
                                     << whitespace(1),
                                 key_value("scaleH", integer()) << whitespace(1)
                             ),
                             key_value("pages", integer()) << whitespace(1),
                             key_value("packed", integer()).map_type<bool>()
                         );

    auto constexpr parse_page_header =
        prefix("page") >> whitespace(1) >>
        collect<FontPageHeader>(
            key_value("id", integer()) << whitespace(1),
            key_value("file", quoted_string()).map_type<std::string>()
        );

    auto constexpr parse_chars_header =
        prefix("chars") >>
        whitespace(1) >> collect<FontCharsHeader>(key_value("count", integer())
                         );

    auto constexpr parse_char_desc =
        prefix("char") >>
        whitespace(1) >> collect<FontCharDesc>(
                             key_value("id", integer()) << whitespace(1),
                             collect<glm::uvec2>(
                                 key_value("x", integer()) << whitespace(1),
                                 key_value("y", integer()) << whitespace(1)
                             ),
                             collect<glm::uvec2>(
                                 key_value("width", integer()) << whitespace(1),
                                 key_value("height", integer()) << whitespace(1)
                             ),
                             collect<glm::ivec2>(
                                 key_value("xoffset", integer())
                                     << whitespace(1),
                                 key_value("yoffset", integer())
                                     << whitespace(1)
                             ),
                             key_value("xadvance", integer()) << whitespace(1),
                             key_value("page", integer()) << whitespace(1),
                             key_value("chnl", integer())
                         );

    auto constexpr parse_kerning =
        prefix("kerning") >>
        whitespace(1) >> collect<FontKerning>(
                             key_value("first", integer()) << whitespace(1),
                             key_value("second", integer()) << whitespace(1),
                             key_value("amount", integer()) << whitespace(1)
                         );

    auto constexpr parse_kernings_header =
        prefix("kernings") >> whitespace(1) >>
        key_value("count", integer()).map_type<FontKerningsHeader>();

    auto constexpr parse_page = collect<FontPage>(
        parse_page_header << whitespace(), parse_chars_header << whitespace(),
        list(parse_char_desc, whitespace()).map([](auto unordered) {
            auto ordered = std::vector<FontCharDesc>(1 << CHAR_BIT);

            for (auto&& desc : unordered) {
                ordered[desc.id] = std::move(desc);
            }

            return ordered;
        }),
        parse_kernings_header.opt_default() << whitespace(),
        list(parse_kerning, whitespace()).opt_default()
    );

    auto constexpr parse_font = collect<Font>(
        parse_info << newline(), parse_common << newline(),
        auto{parse_page}.map([](auto first) { return std::vector{std::move(first)}; })
    );

}  // namespace parser

}  // namespace tmine
