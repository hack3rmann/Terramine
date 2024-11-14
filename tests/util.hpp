#pragma once

#include <concepts>
#include <string_view>
#include <fmt/printf.h>
#include <fmt/color.h>

#define perform_test(function_name) \
    perform_test_impl(function_name, #function_name)

template <std::invocable F>
inline auto perform_test_impl(F test, std::string_view name) -> void {
    try {
        fmt::print(stderr, "test {:.^48}", name);

        test();

        fmt::print(stderr, fmt::fg(fmt::color::lime_green), " passed\n");
    } catch (std::runtime_error const& error) {
        fmt::print(stderr, fmt::fg(fmt::color::red), " failed");
        fmt::print(stderr, ":\n");

        fmt::print(stderr, "    {}\n", error.what());

        exit(1);
    }
}
