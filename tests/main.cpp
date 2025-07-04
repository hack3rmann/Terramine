#include "util.hpp"
#include "parse.hpp"
#include "vec.hpp"
#include "other.hpp"

using namespace tmine_test;

auto main() -> int {
    perform_test(test_parse_fnt_key_value_string);
    perform_test(test_parse_fnt_info);
    perform_test(test_parse_fnt_common);
    perform_test(test_parse_fnt_page);
    perform_test(test_parse_fnt_char_desc);
    perform_test(test_parse_fnt_font);
    perform_test(test_vec_append);
    perform_test(test_vec_append_single_thread);
    perform_test(test_vec_erase_simple);
    perform_test(test_vec_erase);
    perform_test(test_smallvec_push);
    perform_test(test_dynamic_cast_if_init);
}
