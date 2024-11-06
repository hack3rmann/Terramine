#include "loaders.hpp"

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
    auto const result =
        tmine::fnt::parse_key_value_string("key=\"value\" tail", "key");

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

auto test_parse_fnt_common() -> void {
    auto const src =
        "common lineHeight=265 base=189 scaleW=1024 scaleH=1024 pages=1 "
        "packed=0";

    auto const result = tmine::fnt::parse_common(src);

    tmine_assert(result.ok());

    auto const common = std::move(result).get_value();

    tmine_assert_eq(common.line_height, 265);
    tmine_assert_eq(common.base, 189);
    tmine_assert_eq(common.scale.x, 1024);
    tmine_assert_eq(common.scale.y, 1024);
    tmine_assert_eq(common.n_pages, 1);
    tmine_assert_eq(common.is_packed, false);
}

auto test_parse_fnt_page() -> void {
    auto const src = "page id=0 file=\"font.png\"";

    auto const result = tmine::fnt::parse_page_header(src);

    tmine_assert(result.ok());

    auto const page = std::move(result).get_value();

    tmine_assert_eq(page.id, 0);
    tmine_assert_eq(page.file, "font.png");
}

auto test_parse_fnt_char_desc() -> void {
    auto const src =
        "char id=0       x=827  y=481  width=70   height=115  xoffset=3    "
        "yoffset=75   xadvance=75   page=0    chnl=0";

    auto const result = tmine::fnt::parse_char_desc(src);

    tmine_assert(result.ok());

    auto symbol = std::move(result).get_value();

    tmine_assert_eq(symbol.id, 0);
    tmine_assert_eq(symbol.pos.x, 827);
    tmine_assert_eq(symbol.pos.y, 481);
    tmine_assert_eq(symbol.size.x, 70);
    tmine_assert_eq(symbol.size.y, 115);
    tmine_assert_eq(symbol.offset.x, 3);
    tmine_assert_eq(symbol.offset.y, 75);
    tmine_assert_eq(symbol.horizontal_advance, 75);
    tmine_assert_eq(symbol.page_index, 0);
    tmine_assert_eq(symbol.channel, 0);
}

auto test_parse_fnt_font() -> void {
    auto const src = tmine::read_to_string("assets/fonts/font.fnt");
    auto result = tmine::fnt::parse_font(src);

    tmine_assert(result.ok());

    auto const font = std::move(result).get_value();

    tmine_assert_eq(font.pages.size(), 1);
    tmine_assert_eq(font.info.face, "Segoe Print");
    tmine_assert_eq(font.pages[0].chars.size(), 97);
    tmine_assert_eq(font.pages[0].chars[0].size.x, 70);
}

}  // namespace tmine_test
