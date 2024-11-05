#include <fmt/ranges.h>
#include "../assert.hpp"
#include "../parse.hpp"

namespace tmine_test {

using namespace tmine::parser;
using namespace tmine;

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

auto test_parse_parser_map() -> void {
    enum class State {
        First,
        Second,
        Third,
    };

    auto parser = sequence("first").map([](auto) { return State::First; }) |
                  sequence("second").map([](auto) { return State::Second; }) |
                  sequence("third").map([](auto) { return State::Third; });

    auto result1 = parser.parse("first_tail");

    tmine_assert(result1.ok());
    tmine_assert_eq((u32) result1.get_value(), (u32) State::First);
    tmine_assert_eq(result1.tail, "_tail");

    auto result2 = parser.parse("second_tail");

    tmine_assert(result2.ok());
    tmine_assert_eq((u32) result2.get_value(), (u32) State::Second);
    tmine_assert_eq(result2.tail, "_tail");

    auto result3 = parser.parse("third_tail");

    tmine_assert(result3.ok());
    tmine_assert_eq((u32) result3.get_value(), (u32) State::Third);
    tmine_assert_eq(result3.tail, "_tail");
}

auto test_parse_parser_vector_sequence() -> void {
    auto parser = (character('a') | character('b')).sequence();

    auto result = parser.parse("ababbcaba");

    tmine_assert(result.ok());
    tmine_assert_eq(
        result.get_value(), (std::vector<char>{'a', 'b', 'a', 'b', 'b'})
    );
    tmine_assert_eq(result.tail, "caba");
}

}  // namespace tmine_test
