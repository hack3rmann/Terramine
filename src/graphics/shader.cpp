#include <fstream>
#include <iostream>
#include <sstream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "../graphics.hpp"

namespace tmine {

ShaderProgram::ShaderProgram(GLuint id)
: id{id}
, n_clones_ptr{new usize{1}} {}

ShaderProgram::~ShaderProgram() {
    if (ShaderProgram::DUMMY_ID == this->id) {
        return;
    } else if (*this->n_clones_ptr > 1) {
        *this->n_clones_ptr -= 1;
        return;
    }

    glDeleteProgram(id);
}

ShaderProgram::ShaderProgram(ShaderProgram const& other)
: id{other.id}
, n_clones_ptr{other.n_clones_ptr} {
    *other.n_clones_ptr += 1;
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
: id{other.id}
, n_clones_ptr{other.n_clones_ptr} {
    other.id = ShaderProgram::DUMMY_ID;
    other.n_clones_ptr = nullptr;
}

auto ShaderProgram::operator=(
    this ShaderProgram& self, ShaderProgram const& other
) -> ShaderProgram& {
    self.id = other.id;
    self.n_clones_ptr = other.n_clones_ptr;

    *self.n_clones_ptr += 1;

    return self;
}

auto ShaderProgram::operator=(
    this ShaderProgram& self, ShaderProgram&& other
) noexcept -> ShaderProgram& {
    self.id = other.id;
    self.n_clones_ptr = other.n_clones_ptr;

    other.id = ShaderProgram::DUMMY_ID;
    other.n_clones_ptr = nullptr;

    return self;
}

auto ShaderProgram::bind(this ShaderProgram const& self) -> void {
    if (ShaderProgram::DUMMY_ID == self.id) {
        return;
    }

    glUseProgram(self.id);
}

auto ShaderProgram::uniform_mat4(
    this ShaderProgram const& self, char const* name, glm::mat4 matrix
) -> void {
    auto const location = glGetUniformLocation(self.id, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

auto ShaderProgram::uniform_vec2(
    this ShaderProgram const& self, char const* name, glm::vec2 vec
) -> void {
    auto const location = glGetUniformLocation(self.id, name);
    glUniform2f(location, vec.x, vec.y);
}

auto ShaderProgram::uniform_vec3(
    this ShaderProgram const& self, char const* name, glm::vec3 vec
) -> void {
    auto const location = glGetUniformLocation(self.id, name);
    glUniform3f(location, vec.x, vec.y, vec.z);
}

auto ShaderProgram::uniform_int(
    this ShaderProgram const& self, char const* name, i32 num
) -> void {
    auto const location = glGetUniformLocation(self.id, name);
    glUniform1i(location, num);
}

auto ShaderProgram::from_source(ShaderSource const& source
) -> std::optional<ShaderProgram> {
    auto success = (int) true;
    char error_message[512];

    auto const vertex_id = glCreateShader(GL_VERTEX_SHADER);

    auto const vertex_data_ptr = source.vertex.data();
    glShaderSource(vertex_id, 1, &vertex_data_ptr, nullptr);

    glCompileShader(vertex_id);
    glGetShaderiv(vertex_id, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(
            vertex_id, sizeof(error_message), nullptr, error_message
        );

        std::fprintf(
            stderr, "failed to compile vertex shader '%s': %s\n",
            source.vertex_path.c_str(), error_message
        );

        glDeleteShader(vertex_id);

        return std::nullopt;
    }

    auto const fragment_id = glCreateShader(GL_FRAGMENT_SHADER);

    auto const fragment_data_ptr = source.fragment.data();
    glShaderSource(fragment_id, 1, &fragment_data_ptr, nullptr);

    glCompileShader(fragment_id);
    glGetShaderiv(fragment_id, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(
            fragment_id, sizeof(error_message), nullptr, error_message
        );

        std::fprintf(
            stderr, "failed to compile fragment shader '%s': %s\n",
            source.fragment_path.c_str(), error_message
        );

        glDeleteShader(vertex_id);
        glDeleteShader(fragment_id);

        return std::nullopt;
    }

    auto const program_id = glCreateProgram();
    glAttachShader(program_id, vertex_id);
    glAttachShader(program_id, fragment_id);
    glLinkProgram(program_id);

    glGetProgramiv(program_id, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(
            program_id, sizeof(error_message), nullptr, error_message
        );

        std::fprintf(
            stderr,
            "failed to create shader program from vertex shader '%s' and "
            "fragment shader '%s': %s\n",
            source.vertex_path.c_str(), source.fragment_path.c_str(),
            error_message
        );

        glDeleteShader(vertex_id);
        glDeleteShader(fragment_id);
        glDeleteProgram(program_id);

        return std::nullopt;
    }

    glDeleteShader(vertex_id);
    glDeleteShader(fragment_id);

    return ShaderProgram{program_id};
}

}  // namespace tmine
