#include "../assert.hpp"
#include "../parse.hpp"

namespace tmine_test {

auto test_parse_string() -> void {
    auto const result1 = tmine::parse_string("hello, world!", "hello");

    tmine_assert(result1.value.has_value());
    tmine_assert_eq(result1.value.value(), "hello");

    auto const result2 = tmine::parse_string("Hello, World!", "Minecraft");

    tmine_assert(!result2.value.has_value());
}

auto test_parse_char() -> void {
    auto const result1 = tmine::parse_char("Terramine", 'T');

    tmine_assert(result1.value.has_value());
    tmine_assert_eq(result1.value.value(), 'T');

    auto const result2 = tmine::parse_char("Minecraft", 'A');

    tmine_assert(!result2.value.has_value());
}

}
