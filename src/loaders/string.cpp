#include <cstdio>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <fmt/format.h>

#include "../loaders.hpp"

namespace tmine {

int constexpr FSEEK_FAILURE = -1;

auto read_to_string(char const* path) -> std::string {
    auto const file = std::fopen(path, "r");

    if (nullptr == file) {
        throw std::runtime_error(fmt::format(
            "failed to open file '{}': {}", path, std::strerror(errno)
        ));
    }

    if (FSEEK_FAILURE == std::fseek(file, 0, SEEK_END)) {
        std::fclose(file);

        throw std::runtime_error(fmt::format(
            "failed to seek in file '{}': {}", path, std::strerror(errno)
        ));
    }

    auto const file_size = (isize) std::ftell(file);

    if (file_size < 0) {
        std::fclose(file);

        throw std::runtime_error(fmt::format(
            "failed to find out the size of file '{}': {}", path,
            std::strerror(errno)
        ));
    }

    if (FSEEK_FAILURE == std::fseek(file, 0, SEEK_SET)) {
        std::fclose(file);

        throw std::runtime_error(fmt::format(
            "failed to seek in file '{}': {}", path, std::strerror(errno)
        ));
    }

    auto result = std::string((usize) file_size, '\0');

    if (result.size() !=
        std::fread(result.data(), sizeof(result[0]), result.size(), file))
    {
        std::fclose(file);

        throw std::runtime_error(
            fmt::format("failed to read from file '{}'", path)
        );
    }

    std::fclose(file);

    return result;
}

}  // namespace tmine
