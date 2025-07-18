#pragma once

#include <fmt/format.h>
#include "panic.hpp"

#define tmine_assert(expr, ...)                                              \
    ({                                                                       \
        if (!(expr)) {                                                       \
            throw Panic(                                                     \
                "test '{}' failed: assetion '{}' failed" __VA_OPT__(": {}"), \
                __FUNCTION__, #expr __VA_OPT__(, fmt::format(__VA_ARGS__))   \
            );                                                               \
        }                                                                    \
    })

#define tmine_assert_eq(left, right, ...)                                      \
    ({                                                                         \
        auto const left_result = (left);                                       \
        auto const right_result = (right);                                     \
                                                                               \
        if (left_result != right_result) {                                     \
            throw Panic(                                                       \
                "test '{}' failed: assertion '{} == {}' "                      \
                "failed with left = '{}' and right = '{}'" __VA_OPT__(": {}"), \
                __FUNCTION__, #left, #right, left_result,                      \
                right_result __VA_OPT__(, fmt::format(__VA_ARGS__))            \
            );                                                                 \
        }                                                                      \
    })

#define tmine_assert_ne(left, right, ...)                                      \
    ({                                                                         \
        auto const left_result = (left);                                       \
        auto const right_result = (right);                                     \
                                                                               \
        if (left_result == right_result) {                                     \
            throw Panic(                                                       \
                "test '{}' failed: assertion '{} != {}' "                      \
                "failed with left = '{}' and right = '{}'" __VA_OPT__(": {}"), \
                __FUNCTION__, #left, #right, left_result,                      \
                right_result __VA_OPT__(, fmt::format(__VA_ARGS__))            \
            );                                                                 \
        }                                                                      \
    })
