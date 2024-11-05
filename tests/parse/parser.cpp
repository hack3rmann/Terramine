#include "../assert.hpp"
#include "../parse.hpp"

namespace tmine_test {

using namespace tmine::parser;

auto test_parse_parser_sequence() -> void {
    auto parser = sequence("Hello") | sequence("Goodbye");

    auto result1 = parser.parse("Hello, World!");

    tmine_assert(result1.ok());
    tmine_assert_eq(result1.get_value(), "Hello");
    tmine_assert_eq(result1.tail, ", World!");

    auto result2 = parser.parse("Goodbye, World!");

    tmine_assert(result2.ok());
    tmine_assert_eq(result2.get_value(), "Goodbye");
    tmine_assert_eq(result2.tail, ", World!");
}

auto test_parse_parser_right() -> void {
    auto parser = character('=') >> sequence("value");

    auto result1 = parser.parse("=value tail");

    tmine_assert(result1.ok());
    tmine_assert_eq(result1.get_value(), "value");
    tmine_assert_eq(result1.tail, " tail");

    auto result2 = parser.parse("value tail");

    tmine_assert(!result2.ok());

    auto result3 = parser.parse("=novalue");

    tmine_assert(!result3.ok());
}

auto test_parse_parser_left_right() -> void {
    auto parser = character('<') >> sequence("value") << character('>');

    auto result1 = parser.parse("<value>tail");

    tmine_assert(result1.ok());
    tmine_assert_eq(result1.get_value(), "value");
    tmine_assert_eq(result1.tail, "tail");

    auto result2 = parser.parse("<valuetail");

    tmine_assert(!result2.ok());

    auto result3 = parser.parse("value>tail");

    tmine_assert(!result3.ok());
}

}  // namespace tmine_test
