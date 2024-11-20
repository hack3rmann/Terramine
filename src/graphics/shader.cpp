#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "../graphics.hpp"
#include "../panic.hpp"

namespace tmine {

ShaderData::~ShaderData() { glDeleteProgram(this->id); }

ShaderProgram::ShaderProgram(GLuint id)
: data{std::make_shared<ShaderData>(id)} {}

auto ShaderProgram::bind(this ShaderProgram const& self) -> void {
    if (nullptr == self.data) {
        return;
    }

    glUseProgram(self.data->id);
}

auto ShaderProgram::uniform_mat4(
    this ShaderProgram const& self, char const* name, glm::mat4 matrix
) -> void {
    auto const location = glGetUniformLocation(self.data->id, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

auto ShaderProgram::uniform_vec2(
    this ShaderProgram const& self, char const* name, glm::vec2 vec
) -> void {
    auto const location = glGetUniformLocation(self.data->id, name);
    glUniform2f(location, vec.x, vec.y);
}

auto ShaderProgram::uniform_vec3(
    this ShaderProgram const& self, char const* name, glm::vec3 vec
) -> void {
    auto const location = glGetUniformLocation(self.data->id, name);
    glUniform3f(location, vec.x, vec.y, vec.z);
}

auto ShaderProgram::uniform_int(
    this ShaderProgram const& self, char const* name, i32 num
) -> void {
    auto const location = glGetUniformLocation(self.data->id, name);
    glUniform1i(location, num);
}

auto ShaderProgram::uniform_float(
    this ShaderProgram const& self, char const* name, f32 value
) -> void {
    auto const location = glGetUniformLocation(self.data->id, name);
    glUniform1f(location, value);
}

auto ShaderProgram::from_source(ShaderSource const& source) -> ShaderProgram {
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

        glDeleteShader(vertex_id);

        throw Panic(
            "failed to compile vertex shader '{}': {}",
            source.vertex_path.c_str(), error_message
        );
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

        glDeleteShader(vertex_id);
        glDeleteShader(fragment_id);

        throw Panic(
            "failed to compile fragment shader '{}': {}",
            source.fragment_path.c_str(), error_message
        );
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

        glDeleteShader(vertex_id);
        glDeleteShader(fragment_id);
        glDeleteProgram(program_id);

        throw Panic(
            "failed to create shader program from vertex shader '{}' and "
            "fragment shader '{}': {}\n",
            source.vertex_path.c_str(), source.fragment_path.c_str(),
            error_message
        );
    }

    glDeleteShader(vertex_id);
    glDeleteShader(fragment_id);

    return ShaderProgram{program_id};
}

}  // namespace tmine
