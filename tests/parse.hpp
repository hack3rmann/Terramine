#pragma once

#include <parser.hpp>

namespace tmine_test {

auto test_parse_sequence() -> void;
auto test_parse_char() -> void;
auto test_parse_combine() -> void;
auto test_parse_integer() -> void;
auto test_parse_whitespaces() -> void;
auto test_parse_newline() -> void;
auto test_parse_string() -> void;
auto test_parse_parser_sequence() -> void;
auto test_parse_parser_right() -> void;
auto test_parse_parser_left_right() -> void;
auto test_parse_parser_map() -> void;
auto test_parse_parser_vector_sequence() -> void;
auto test_parse_fnt_key_value_string() -> void;
auto test_parse_fnt_info() -> void;
auto test_parse_fnt_common() -> void;
auto test_parse_fnt_page() -> void;
auto test_parse_fnt_char_desc() -> void;
auto test_parse_fnt_font() -> void;

}  // namespace tmine_test
