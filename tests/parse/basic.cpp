#include "../assert.hpp"
#include "../parse.hpp"

namespace tmine_test {

auto test_parse_sequence() -> void {
    auto const result1 = tmine::parse_sequence("hello, world!", "hello");

    tmine_assert(result1.ok());
    tmine_assert_eq(result1.get_value(), "hello");
    tmine_assert_eq(result1.tail, ", world!");

    auto const result2 = tmine::parse_sequence("Hello, World!", "Minecraft");

    tmine_assert(!result2.ok());
}

auto test_parse_char() -> void {
    auto const result1 = tmine::parse_char("Terramine", 'T');

    tmine_assert(result1.ok());
    tmine_assert_eq(result1.get_value(), 'T');
    tmine_assert_eq(result1.tail, "erramine");

    auto const result2 = tmine::parse_char("Minecraft", 'A');

    tmine_assert(!result2.ok());
}

auto test_parse_combine() -> void {
    auto const src = "Minecraft is a good game";

    auto const result1 = tmine::parse_sequence(src, "Terraria") |
                         tmine::parse_sequence(src, "Minecraft");

    tmine_assert(result1.ok());
    tmine_assert_eq(result1.get_value(), "Minecraft");
    tmine_assert_eq(result1.tail, " is a good game");

    auto const result2 = tmine::parse_sequence(src, "Minecraft") |
                         tmine::parse_sequence(src, "Terraria");

    tmine_assert(result2.ok());
    tmine_assert_eq(result2.get_value(), "Minecraft");
    tmine_assert_eq(result2.tail, " is a good game");

    auto const result3 = tmine::parse_sequence(src, "Terraria") |
                         tmine::parse_sequence(src, "VintageStory");

    tmine_assert(!result3.ok());
}

auto test_parse_integer() -> void {
    auto const result1 = tmine::parse_integer("42");

    tmine_assert(result1.ok());
    tmine_assert_eq(result1.get_value(), 42);
    tmine_assert_eq(result1.tail, "");

    auto const result2 = tmine::parse_integer("1234567 is a number");

    tmine_assert(result2.ok());
    tmine_assert_eq(result2.get_value(), 1234567);
    tmine_assert_eq(result2.tail, " is a number");

    auto const result3 = tmine::parse_integer("Hello, Wolrd!");

    tmine_assert(!result3.ok());

    auto const result4 = tmine::parse_integer("-666");

    tmine_assert(result4.ok());
    tmine_assert_eq(result4.get_value(), -666);
    tmine_assert_eq(result4.tail, "");
}

auto test_parse_whitespaces() -> void {
    auto const result1 = tmine::parse_whitespace("  \t\nName");

    tmine_assert(result1.ok());
    tmine_assert_eq(result1.get_value(), "  \t\n");
    tmine_assert_eq(result1.tail, "Name");

    auto const result2 = tmine::parse_whitespace("Name");

    tmine_assert(result2.ok());
    tmine_assert_eq(result2.get_value(), "");
    tmine_assert_eq(result2.tail, "Name");

    auto const result3 = tmine::parse_whitespace("Name", 1);

    tmine_assert(!result3.ok());

    auto const result4 = tmine::parse_whitespace(" Number", 2);

    tmine_assert(!result4.ok());

    auto const result5 = tmine::parse_whitespace(" \n Number", 2);

    tmine_assert(result5.ok());
    tmine_assert_eq(result5.get_value(), " \n ");
    tmine_assert_eq(result5.tail, "Number");
}

auto test_parse_newline() -> void {
    auto const result1 = tmine::parse_newline("\nNew line");

    tmine_assert(result1.ok());
    tmine_assert_eq(result1.get_value(), "\n");
    tmine_assert_eq(result1.tail, "New line");

    auto const result2 = tmine::parse_newline("\r\nNew line");

    tmine_assert(result2.ok());
    tmine_assert_eq(result2.get_value(), "\r\n");
    tmine_assert_eq(result2.tail, "New line");
}

}  // namespace tmine_test
