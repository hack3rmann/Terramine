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
    auto tail = std::string_view{};

    try_parse(sequence, tail, src, "info");
    try_parse_argless(whitespace, tail, tail);

    auto const face = try_parse(key_value_string, tail, tail, "face");
    try_parse_argless(whitespace, tail, tail);

    auto const size = try_parse(key_value_integer, tail, tail, "size");
    try_parse_argless(whitespace, tail, tail);

    auto const bold = try_parse(key_value_integer, tail, tail, "bold");
    try_parse_argless(whitespace, tail, tail);

    auto const italic = try_parse(key_value_integer, tail, tail, "italic");
    try_parse_argless(whitespace, tail, tail);

    auto const charset = try_parse(key_value_string, tail, tail, "charset");
    try_parse_argless(whitespace, tail, tail);

    auto const unicode = try_parse(key_value_integer, tail, tail, "unicode");
    try_parse_argless(whitespace, tail, tail);

    auto const stretch_h = try_parse(key_value_integer, tail, tail, "stretchH");
    try_parse_argless(whitespace, tail, tail);

    auto const smooth = try_parse(key_value_integer, tail, tail, "smooth");
    try_parse_argless(whitespace, tail, tail);

    auto const aa = try_parse(key_value_integer, tail, tail, "aa");
    try_parse_argless(whitespace, tail, tail);

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

    return ParseResult<Info> {
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

}  // namespace fnt
}  // namespace tmine
