#include "../loaders.hpp"

namespace tmine {

namespace fs = std::filesystem;

auto load_shader_source(
    char const* vertex_source_path, char const* fragment_source_path
) -> ShaderSource {
    auto vertex_source =
        read_to_string((fs::path{SHADERS_PATH} / vertex_source_path).c_str());

    auto fragment_source =
        read_to_string((fs::path{SHADERS_PATH} / fragment_source_path).c_str());

    return ShaderSource{
        .vertex = std::move(vertex_source),
        .fragment = std::move(fragment_source),
        .vertex_path = std::move(vertex_source_path),
        .fragment_path = std::move(fragment_source_path),
    };
}

auto load_shader(
    char const* vertex_source_path, char const* fragment_source_path
) -> ShaderProgram {
    auto const source =
        load_shader_source(vertex_source_path, fragment_source_path);

    return ShaderProgram::from_source(source);
}

}  // namespace tmine
