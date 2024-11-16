#pragma once

#include <fmt/printf.h>

#define tmine_log(args...) ::fmt::print(stderr, args)
