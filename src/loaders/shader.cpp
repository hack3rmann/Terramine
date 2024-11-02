#include "../loaders.hpp"

namespace tmine {

namespace fs = std::filesystem;

auto load_shader_source(
    char const* vertex_source_path, char const* fragment_source_path
) -> std::optional<ShaderSource> {
    auto vertex_source =
        read_to_string((fs::path{SHADERS_PATH} / vertex_source_path).c_str());

    if (!vertex_source.has_value()) {
        std::fprintf(
            stderr, "failed to load vertex shader '%s'\n", vertex_source_path
        );

        return std::nullopt;
    }

    auto fragment_source =
        read_to_string((fs::path{SHADERS_PATH} / fragment_source_path).c_str());

    if (!fragment_source.has_value()) {
        std::fprintf(
            stderr, "failed to load fragmen shader '%s'\n", fragment_source_path
        );

        return std::nullopt;
    }

    return ShaderSource{
        .vertex = std::move(vertex_source).value(),
        .fragment = std::move(fragment_source).value(),
        .vertex_path = std::move(vertex_source_path),
        .fragment_path = std::move(fragment_source_path),
    };
}

auto load_shader(
    char const* vertex_source_path, char const* fragment_source_path
) -> std::optional<ShaderProgram> {
    auto const source =
        load_shader_source(vertex_source_path, fragment_source_path);

    if (!source.has_value()) {
        return std::nullopt;
    }

    return ShaderProgram::from_source(source.value());
}

}  // namespace tmine
