#include "util.hpp"
#include "parse.hpp"

using namespace tmine_test;

auto main() -> int {
    perform_test(test_parse_char);
    perform_test(test_parse_string);
}
