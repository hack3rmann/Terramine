#include "../loaders.hpp"

namespace tmine {

namespace fs = std::filesystem;

auto load_shader_source(
    fs::path vertex_source_path, fs::path fragment_source_path
) -> std::optional<ShaderSource> {
    auto vertex_source =
        read_to_string((SHADERS_PATH / vertex_source_path).c_str());

    if (!vertex_source.has_value()) {
        std::fprintf(
            stderr, "failed to load vertex shader '%s'\n",
            vertex_source_path.c_str()
        );

        return std::nullopt;
    }

    auto fragment_source =
        read_to_string((SHADERS_PATH / fragment_source_path).c_str());

    if (!fragment_source.has_value()) {
        std::fprintf(
            stderr, "failed to load fragmen shader '%s'\n",
            fragment_source_path.c_str()
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
    std::filesystem::path vertex_source_path,
    std::filesystem::path fragment_source_path
) -> std::optional<ShaderProgram> {
    auto const source = load_shader_source(
        std::move(vertex_source_path), std::move(fragment_source_path)
    );

    if (!source.has_value()) {
        return std::nullopt;
    }

    return ShaderProgram::from_source(source.value());
}

}  // namespace tmine
