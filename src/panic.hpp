#pragma once

#include <optional>
#include <fmt/format.h>
#include "types.hpp"

#define Panic(fmt_args...)                                        \
    ::tmine::PanicException {                                     \
        __FUNCTION__, __FILE__, __LINE__, ::fmt::format(fmt_args) \
    }

#define Unimplemented(...) Panic("unimplemented" __VA_OPT__(": ") __VA_ARGS__)

namespace tmine {

class FormattableException : public std::exception {
public:
    virtual ~FormattableException() = default;

    virtual auto get_formatted_message() const -> std::string = 0;

    inline auto what() const noexcept -> char const* override {
        if (!this->message.has_value()) {
            this->message.emplace(this->get_formatted_message());
        }

        return this->message->c_str();
    }

private:
    mutable std::optional<std::string> message;
};

class PanicException : public FormattableException {
public:
    inline PanicException(
        std::string_view function_name, std::string_view file_name,
        usize line_number, std::string message
    )
    : function_name{function_name}
    , file_name{file_name}
    , line_number{line_number}
    , message{std::move(message)} {}

    inline auto get_formatted_message() const -> std::string override {
        return fmt::format(
            "panic in function `{}` in file `{}:{}`: {}", this->function_name,
            this->file_name, this->line_number, this->message
        );
    }

private:
    std::string_view function_name;
    std::string_view file_name;
    usize line_number;
    std::string message;
};

}  // namespace tmine
