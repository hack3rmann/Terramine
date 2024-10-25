#include <cstdio>
#include <cerrno>
#include <cstring>

#include "../loaders.hpp"

namespace tmine {

int constexpr FSEEK_FAILURE = -1;

auto read_to_string(std::filesystem::path path) -> std::optional<std::string> {
    auto const file = std::fopen(path.c_str(), "r");

    if (nullptr == file) {
        std::fprintf(
            stderr, "failed to open file '%s': %s\n", path.c_str(),
            std::strerror(errno)
        );

        return std::nullopt;
    }

    if (FSEEK_FAILURE == std::fseek(file, 0, SEEK_END)) {
        std::fprintf(
            stderr, "failed to seek in file '%s': %s\n", path.c_str(),
            std::strerror(errno)
        );

        std::fclose(file);

        return std::nullopt;
    }

    auto const file_size = (isize) std::ftell(file);

    if (file_size < 0) {
        std::fprintf(
            stderr, "failed to find out the size of file '%s': %s\n",
            path.c_str(), std::strerror(errno)
        );

        std::fclose(file);

        return std::nullopt;
    }

    if (FSEEK_FAILURE == std::fseek(file, 0, SEEK_SET)) {
        std::fprintf(
            stderr, "failed to seek in file '%s': %s\n", path.c_str(),
            std::strerror(errno)
        );

        std::fclose(file);

        return std::nullopt;
    }

    auto result = std::string((usize) file_size, '\0');

    if (result.size() !=
        std::fread(result.data(), sizeof(result[0]), result.size(), file))
    {
        std::fprintf(stderr, "failed to read from file '%s'\n", path.c_str());

        std::fclose(file);

        return std::nullopt;
    }

    std::fclose(file);

    return result;
}

}  // namespace tmine
