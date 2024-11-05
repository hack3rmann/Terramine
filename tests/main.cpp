#include "util.hpp"
#include "parse.hpp"

using namespace tmine_test;

auto main() -> int {
    perform_test(test_parse_char);
    perform_test(test_parse_sequence);
    perform_test(test_parse_combine);
    perform_test(test_parse_integer);
    perform_test(test_parse_whitespaces);
    perform_test(test_parse_newline);
    perform_test(test_parse_string);
    perform_test(test_parse_parser_sequence);
    perform_test(test_parse_parser_right);
    perform_test(test_parse_parser_left_right);
    perform_test(test_parse_fnt_key_value_string);
    perform_test(test_parse_fnt_info);
}
