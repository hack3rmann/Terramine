#include <fmt/printf.h>
#include "../parser.hpp"

#define try_parse_argless(parser_name, tail, src)          \
    ({                                                     \
        auto const result = parse_##parser_name(src);      \
        tail = result.tail;                                \
                                                           \
        if (!result.ok()) {                                \
            return {.value = ::std::nullopt, .tail = src}; \
        }                                                  \
        result.get_value();                                \
    })

#define try_parse(parser_name, tail, src, args...)          \
    ({                                                      \
        auto const result = parse_##parser_name(src, args); \
        tail = result.tail;                                 \
                                                            \
        if (!result.ok()) {                                 \
            return {.value = ::std::nullopt, .tail = src};  \
        }                                                   \
        result.get_value();                                 \
    })

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

using namespace ::tmine::parser;
using namespace comb;

namespace tmine {
namespace fnt {

    auto parse_key_value_integer(std::string_view src, std::string_view key)
        -> comb::ParseResult<i64> {
        auto parser = prefix(key) >> character('=') >> integer();

        return parser.parse(src);
    }

    auto parse_key_value_string(std::string_view src, std::string_view key)
        -> comb::ParseResult<std::string_view> {
        auto parser = prefix(key) >> character('=') >> quoted_string();

        return parser.parse(src);
    }

    auto parse_info(std::string_view src) -> comb::ParseResult<FontInfo> {
        auto tail = std::string_view{};

        parse_unwrap(tail, (prefix("info") << whitespace(1))(src));

        auto const face = parse_unwrap(
            tail, (fnt_key_value_string("face") << whitespace(1))(tail)
        );

        auto const size = parse_unwrap(
            tail, (fnt_key_value_integer("size") << whitespace(1))(tail)
        );

        auto const bold = parse_unwrap(
            tail, (fnt_key_value_integer("bold") << whitespace(1))(tail)
        );

        auto const italic = parse_unwrap(
            tail, (fnt_key_value_integer("italic") << whitespace(1))(tail)
        );

        auto const charset = parse_unwrap(
            tail, (fnt_key_value_string("charset") << whitespace(1))(tail)
        );

        auto const unicode = parse_unwrap(
            tail, (fnt_key_value_integer("unicode") << whitespace(1))(tail)
        );

        auto const stretch_h = parse_unwrap(
            tail, (fnt_key_value_integer("stretchH") << whitespace(1))(tail)
        );

        auto const smooth = parse_unwrap(
            tail, (fnt_key_value_integer("smooth") << whitespace(1))(tail)
        );

        auto const aa = parse_unwrap(
            tail, (fnt_key_value_integer("aa") << whitespace(1))(tail)
        );

        auto const padding_x = execute_parser(
            tail, prefix("padding") >> character('=') >> integer()
        );
        auto const padding_y =
            execute_parser(tail, character(',') >> integer());
        auto const padding_z =
            execute_parser(tail, character(',') >> integer());
        auto const padding_w =
            execute_parser(tail, character(',') >> integer() << whitespace());

        auto const spacing_x = execute_parser(
            tail, prefix("spacing") >> character('=') >> integer()
        );
        auto const spacing_y =
            execute_parser(tail, character(',') >> integer());

        return comb::ParseResult<FontInfo>{
            .value =
                FontInfo{
                    .face = std::string{face},
                    .charset = std::string{charset},
                    .size = (u32) size,
                    .horizontal_stretch = (u32) stretch_h,
                    .is_bold = 0 != bold,
                    .is_italic = 0 != italic,
                    .is_unicode = 0 != unicode,
                    .is_smooth = 0 != smooth,
                    .is_antialiased = 0 != aa,
                    .padding =
                        glm::ivec4{padding_x, padding_y, padding_z, padding_w},
                    .spacing = glm::ivec2{spacing_x, spacing_y},
                },
            .tail = tail,
        };
    }

    auto parse_common(std::string_view src) -> comb::ParseResult<FontCommon> {
        auto tail = src;

        execute_parser(tail, prefix("common") << whitespace(1));

        auto const line_height = execute_parser(
            tail, fnt_key_value_integer("lineHeight") << whitespace(1)
        );
        auto const base = execute_parser(
            tail, fnt_key_value_integer("base") << whitespace(1)
        );
        auto const scale_width = execute_parser(
            tail, fnt_key_value_integer("scaleW") << whitespace(1)
        );
        auto const scale_height = execute_parser(
            tail, fnt_key_value_integer("scaleH") << whitespace(1)
        );
        auto const n_pages = execute_parser(
            tail, fnt_key_value_integer("pages") << whitespace(1)
        );
        auto const packed =
            execute_parser(tail, fnt_key_value_integer("packed"));

        return comb::ParseResult<FontCommon>{
            .value =
                FontCommon{
                    .line_height = (u32) line_height,
                    .base = (u32) base,
                    .scale = glm::uvec2{scale_width, scale_height},
                    .n_pages = (u32) n_pages,
                    .is_packed = 0 != packed,
                },
            .tail = tail,
        };
    }

    auto parse_page_header(std::string_view src)
        -> comb::ParseResult<FontPageHeader> {
        auto tail = src;

        execute_parser(tail, prefix("page") << whitespace(1));

        auto const id =
            execute_parser(tail, fnt_key_value_integer("id") << whitespace(1));
        auto const file = execute_parser(tail, fnt_key_value_string("file"));

        return comb::ParseResult<FontPageHeader>{
            .value =
                FontPageHeader{
                    .file = std::string{file},
                    .id = (u32) id,
                },
            .tail = tail,
        };
    }

    auto parse_chars_header(std::string_view src)
        -> comb::ParseResult<FontCharsHeader> {
        auto tail = src;

        execute_parser(tail, prefix("chars") << whitespace(1));

        auto const count = execute_parser(tail, fnt_key_value_integer("count"));

        return comb::ParseResult<FontCharsHeader>{
            .value =
                FontCharsHeader{
                    .count = (u32) count,
                },
            .tail = tail,
        };
    }

    auto parse_char_desc(std::string_view src)
        -> comb::ParseResult<FontCharDesc> {
        auto tail = src;

        execute_parser(tail, prefix("char") << whitespace(1));

        auto const id =
            execute_parser(tail, fnt_key_value_integer("id") << whitespace(1));

        auto const x =
            execute_parser(tail, fnt_key_value_integer("x") << whitespace(1));
        auto const y =
            execute_parser(tail, fnt_key_value_integer("y") << whitespace(1));
        auto const width = execute_parser(
            tail, fnt_key_value_integer("width") << whitespace(1)
        );
        auto const height = execute_parser(
            tail, fnt_key_value_integer("height") << whitespace(1)
        );
        auto const xoffset = execute_parser(
            tail, fnt_key_value_integer("xoffset") << whitespace(1)
        );
        auto const yoffset = execute_parser(
            tail, fnt_key_value_integer("yoffset") << whitespace(1)
        );
        auto const xadvance = execute_parser(
            tail, fnt_key_value_integer("xadvance") << whitespace(1)
        );
        auto const page = execute_parser(
            tail, fnt_key_value_integer("page") << whitespace(1)
        );

        auto const channel =
            execute_parser(tail, fnt_key_value_integer("chnl"));

        return comb::ParseResult<FontCharDesc>{
            .value =
                FontCharDesc{
                    .id = (u32) id,
                    .pos = glm::uvec2{x, y},
                    .size = glm::uvec2{width, height},
                    .offset = glm::ivec2{xoffset, yoffset},
                    .horizontal_advance = (u32) xadvance,
                    .page_index = (u32) page,
                    .channel = (u32) channel,
                },
            .tail = tail,
        };
    }

    auto parse_kerning(std::string_view src) -> comb::ParseResult<FontKerning> {
        auto tail = src;

        execute_parser(tail, prefix("kerning") << whitespace(1));

        auto const first = execute_parser(
            tail, fnt_key_value_integer("first") << whitespace(1)
        );
        auto const second = execute_parser(
            tail, fnt_key_value_integer("second") << whitespace(1)
        );
        auto const amount = execute_parser(
            tail, fnt_key_value_integer("amount") << whitespace(1)
        );

        return comb::ParseResult<FontKerning>{
            .value =
                FontKerning{
                    .first = (u32) first,
                    .second = (u32) second,
                    .amount = (i32) amount,
                },
            .tail = tail,
        };
    }

    auto parse_page(std::string_view src) -> comb::ParseResult<FontPage> {
        auto tail = src;

        auto const page_header =
            execute_parser(tail, fnt_page_header() << whitespace());

        auto const char_header =
            execute_parser(tail, fnt_chars_header() << whitespace());

        auto const chars_unordered =
            execute_parser(tail, (fnt_char_desc() << whitespace()).repeat());

        auto chars_ordered = std::vector<FontCharDesc>(1 << CHAR_BIT);

        for (auto&& desc : chars_unordered) {
            chars_ordered[desc.id] = std::move(desc);
        }

        auto const kerning_header =
            execute_parser(tail, fnt_kerning_header() << whitespace());

        auto const kernings =
            execute_parser(tail, (fnt_kerning() << whitespace()).repeat());

        return comb::ParseResult<FontPage>{
            .value =
                FontPage{
                    .header = std::move(page_header),
                    .chars_header = std::move(char_header),
                    .kernings_header = std::move(kerning_header),
                    .chars = std::move(chars_ordered),
                    .kernings = std::move(kernings),
                },
            .tail = tail,
        };
    }

    auto parse_font(std::string_view src) -> comb::ParseResult<Font> {
        auto tail = src;

        auto const info = execute_parser(tail, fnt_info() << newline());
        auto const common = execute_parser(tail, fnt_common() << newline());
        auto const pages = std::vector{execute_parser(tail, fnt_page())};

        return comb::ParseResult<Font>{
            .value =
                Font{
                    .info = info,
                    .common = common,
                    .pages = std::move(pages),
                },
            .tail = tail,
        };
    }

}  // namespace fnt
}  // namespace tmine
