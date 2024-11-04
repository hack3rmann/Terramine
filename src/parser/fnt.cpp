#include "../parser.hpp"

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
    tail.remove_suffix(n_string_symbols + 1);

    return ParseResult<std::string_view>{.value = match, .tail = tail};
}

// info face="Segoe Print" size=150 bold=0 italic=0 charset="" unicode=0
// stretchH=100 smooth=1 aa=1 padding=1,1,1,1 spacing=-2,-2

auto parse_info(std::string_view src) -> ParseResult<Info>;

}  // namespace fnt
}  // namespace tmine
