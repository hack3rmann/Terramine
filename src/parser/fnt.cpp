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

namespace tmine {
namespace fnt {
    auto parse_string(std::string_view src) -> ParseResult<std::string_view> {
        auto const open_quote = parse_char(src, '"');
        auto tail = open_quote.tail;

        if (!open_quote.ok()) {
            return ParseResult<std::string_view>{
                .value = std::nullopt, .tail = src
            };
        }

        auto n_string_symbols = usize{0};

        for (auto symbol : tail) {
            if ('"' == symbol) {
                break;
            }

            n_string_symbols += 1;
        }

        if (n_string_symbols == tail.size() || '"' != tail[n_string_symbols]) {
            return ParseResult<std::string_view>{
                .value = std::nullopt, .tail = src
            };
        }

        auto match = tail;
        match.remove_suffix(src.size() - 1 - n_string_symbols);
        tail.remove_prefix(n_string_symbols + 1);

        return ParseResult<std::string_view>{.value = match, .tail = tail};
    }

    auto parse_key_value_integer(std::string_view src, std::string_view key)
        -> ParseResult<i64> {
        using namespace ::tmine::parser;

        auto parser = sequence(key) >> character('=') >> integer();

        return parser.parse(src);
    }

    auto parse_key_value_string(std::string_view src, std::string_view key)
        -> ParseResult<std::string_view> {
        using namespace ::tmine::parser;

        auto parser = sequence(key) >> character('=') >> quoted_string();

        return parser.parse(src);
    }

    auto parse_info(std::string_view src) -> ParseResult<Info> {
        using namespace ::tmine::parser;

        auto tail = std::string_view{};

        parse_unwrap(tail, (sequence("info") << whitespace(1))(src));

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

        try_parse(sequence, tail, tail, "padding");
        try_parse(char, tail, tail, '=');
        auto const padding_x = try_parse_argless(integer, tail, tail);
        try_parse(char, tail, tail, ',');
        auto const padding_y = try_parse_argless(integer, tail, tail);
        try_parse(char, tail, tail, ',');
        auto const padding_z = try_parse_argless(integer, tail, tail);
        try_parse(char, tail, tail, ',');
        auto const padding_w = try_parse_argless(integer, tail, tail);
        try_parse_argless(whitespace, tail, tail);

        try_parse(sequence, tail, tail, "spacing");
        try_parse(char, tail, tail, '=');
        auto const spacing_x = try_parse_argless(integer, tail, tail);
        try_parse(char, tail, tail, ',');
        auto const spacing_y = try_parse_argless(integer, tail, tail);

        return ParseResult<Info>{
            .value =
                Info{
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

    auto parse_common(std::string_view src) -> ParseResult<Common> {
        using namespace ::tmine::parser;

        auto tail = src;

        execute_parser(tail, sequence("common") << whitespace(1));

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

        return ParseResult<Common>{
            .value =
                Common{
                    .line_height = (u32) line_height,
                    .base = (u32) base,
                    .scale_width = (u32) scale_width,
                    .scale_height = (u32) scale_height,
                    .n_pages = (u32) n_pages,
                    .is_packed = 0 != packed,
                },
            .tail = tail,
        };
    }

    auto parse_page_header(std::string_view src) -> ParseResult<PageHeader> {
        using namespace ::tmine::parser;

        auto tail = src;

        execute_parser(tail, sequence("page") << whitespace(1));

        auto const id =
            execute_parser(tail, fnt_key_value_integer("id") << whitespace(1));
        auto const file = execute_parser(tail, fnt_key_value_string("file"));

        return ParseResult<PageHeader>{
            .value =
                PageHeader{
                    .file = std::string{file},
                    .id = (u32) id,
                },
            .tail = tail,
        };
    }

    auto parse_chars_header(std::string_view src) -> ParseResult<CharsHeader> {
        using namespace ::tmine::parser;

        auto tail = src;

        execute_parser(tail, sequence("chars") << whitespace(1));

        auto const count = execute_parser(tail, fnt_key_value_integer("count"));

        return ParseResult<CharsHeader>{
            .value =
                CharsHeader{
                    .count = (u32) count,
                },
            .tail = tail,
        };
    }

    auto parse_char_desc(std::string_view src) -> ParseResult<CharDesc> {
        using namespace ::tmine::parser;

        auto tail = src;

        execute_parser(tail, sequence("char") << whitespace(1));

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

        return ParseResult<CharDesc>{
            .value =
                CharDesc{
                    .id = (u32) id,
                    .pos = glm::uvec2{x, y},
                    .size = glm::uvec2{width, height},
                    .offset = glm::uvec2{xoffset, yoffset},
                    .horizontal_advance = (u32) xadvance,
                    .page_index = (u32) page,
                    .channel = (u32) channel,
                },
            .tail = tail,
        };
    }

    auto parse_kerning(std::string_view src) -> ParseResult<Kerning> {
        using namespace ::tmine::parser;

        auto tail = src;

        execute_parser(tail, sequence("kerning") << whitespace(1));

        auto const first = execute_parser(
            tail, fnt_key_value_integer("first") << whitespace(1)
        );
        auto const second = execute_parser(
            tail, fnt_key_value_integer("second") << whitespace(1)
        );
        auto const amount = execute_parser(
            tail, fnt_key_value_integer("amount") << whitespace(1)
        );

        return ParseResult<Kerning>{
            .value =
                Kerning{
                    .first = (u32) first,
                    .second = (u32) second,
                    .amount = (i32) amount,
                },
            .tail = tail,
        };
    }

    auto parse_page(std::string_view src) -> ParseResult<Page> {
        using namespace ::tmine::parser;

        auto tail = src;

        auto const page_header =
            execute_parser(tail, fnt_page_header() << whitespace());

        auto const char_header =
            execute_parser(tail, fnt_chars_header() << whitespace());

        auto const chars =
            execute_parser(tail, (fnt_char_desc() << whitespace()).sequence());

        auto const kerning_header =
            execute_parser(tail, fnt_kerning_header() << whitespace());

        auto const kernings =
            execute_parser(tail, (fnt_kerning() << whitespace()).sequence());

        return ParseResult<Page>{
            .value =
                Page{
                    .header = std::move(page_header),
                    .chars_header = std::move(char_header),
                    .kernings_header = std::move(kerning_header),
                    .chars = std::move(chars),
                    .kernings = std::move(kernings),
                },
            .tail = tail,
        };
    }

    auto parse_font(std::string_view src) -> ParseResult<Font> {
        using namespace ::tmine::parser;

        auto tail = src;

        auto const info = execute_parser(tail, fnt_info() << newline());
        auto const common = execute_parser(tail, fnt_common() << newline());
        auto const pages = std::vector{execute_parser(tail, fnt_page())};

        return ParseResult<Font>{
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
