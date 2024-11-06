#pragma once

#include <stdexcept>
#include <fmt/format.h>

#define Panic(fmt_args...)                                              \
    ::tmine::PanicException {                                           \
        ::fmt::format(                                                  \
            "panic in function `{}` in file `{}:{}`: {}", __FUNCTION__, \
            __FILE__, __LINE__, ::fmt::format(fmt_args)                 \
        )                                                               \
    }

namespace tmine {

struct PanicException : public std::runtime_error {
    inline PanicException(std::string const& message)
    : std::runtime_error{message} {}
};

}  // namespace tmine
