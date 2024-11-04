#include "../parse.hpp"
#include "../assert.hpp"

namespace tmine_test {

auto test_parse_string() -> void {
    auto const result1 = tmine::fnt::parse_string("\"String\"");

    tmine_assert(result1.ok());
    tmine_assert_eq(result1.get_value(), "String");
    tmine_assert_eq(result1.tail, "");

    auto const result2 = tmine::fnt::parse_string("\"NotString");

    tmine_assert(!result2.ok());

    auto const result3 = tmine::fnt::parse_string("AlsoNotAString");

    tmine_assert(!result3.ok());

    auto const result4 = tmine::fnt::parse_string("\"\"String!");

    tmine_assert(result4.ok());
    tmine_assert_eq(result4.get_value(), "");
    tmine_assert_eq(result4.tail, "String!");
}

auto test_parse_fnt_key_value_string() -> void {
    auto const result = tmine::fnt::parse_key_value_string("key=\"value\" tail", "key");

    tmine_assert(result.ok());
    tmine_assert_eq(result.get_value(), "value");
    tmine_assert_eq(result.tail, " tail");
}

auto test_parse_fnt_info() -> void {
    auto const src =
        "info face=\"Segoe Print\" size=150 bold=0 italic=0 charset=\"\" "
        "unicode=0 stretchH=100 smooth=1 aa=1 padding=1,1,1,1 spacing=-2,-2";

    auto const result = tmine::fnt::parse_info(src);

    tmine_assert(result.ok());
    tmine_assert_eq(result.tail, "");

    auto const value = std::move(result).get_value();
    
    tmine_assert_eq(value.face, "Segoe Print");
    tmine_assert_eq(value.size, 150);
    tmine_assert_eq(value.is_bold, false);
    tmine_assert_eq(value.is_italic, false);
    tmine_assert_eq(value.charset, "");
    tmine_assert_eq(value.is_unicode, false);
    tmine_assert_eq(value.horizontal_stretch, 100);
    tmine_assert_eq(value.is_smooth, true);
    tmine_assert_eq(value.is_antialiased, true);
    tmine_assert_eq(value.padding.x, 1);
    tmine_assert_eq(value.padding.y, 1);
    tmine_assert_eq(value.padding.z, 1);
    tmine_assert_eq(value.padding.w, 1);
    tmine_assert_eq(value.spacing.x, -2);
    tmine_assert_eq(value.spacing.y, -2);
}

}
