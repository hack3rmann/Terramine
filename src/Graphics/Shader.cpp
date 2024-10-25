#include "Shader.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "../defines.cpp"

Shader::Shader(unsigned int id)
    : id(id) {}

Shader::~Shader() { glcall(glDeleteProgram(id)); }

void Shader::use() { glcall(glUseProgram(id)); }

void Shader::uniformMatrix(char const* name, glm::mat4 matrix) {
    GLuint transformLoc = glGetUniformLocation(id, name);
    glcall(glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matrix))
    );
}

void Shader::uniformVec2u(char const* name, glm::vec2 vec) {
    GLuint loc = glGetUniformLocation(id, name);
    glcall(glUniform2f(loc, vec.x, vec.y));
}

void Shader::uniform3f(char const* name, glm::vec3 vec) {
    GLuint loc = glGetUniformLocation(id, name);
    glcall(glUniform3f(loc, vec.x, vec.y, vec.z));
}

void Shader::uniform1i(char const* name, int num) {
    GLuint loc = glGetUniformLocation(id, name);
    glcall(glUniform1i(loc, num));
}

Shader* load_shader(
    std::string const vertexFile, std::string const fragmentFile
) {
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile, fShaderFile;

    vShaderFile.exceptions(std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::badbit);

    try {
        vShaderFile.open("assets/Shaders/" + vertexFile);
        if (!vShaderFile.is_open()) {
            std::cout << "Vertex shader file not opened!\n";
        }
        fShaderFile.open("assets/Shaders/" + fragmentFile);
        if (!fShaderFile.is_open()) {
            std::cout << "Fragment shader file not opened!\n";
        }
        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure&) {
        std::cerr << "Error : shader file not succesfully read\n";
        return nullptr;
    }

    char const* vShaderCode = vertexCode.c_str();
    char const* fShaderCode = fragmentCode.c_str();
    int success;
    char infolog[512];

    CONSOLE_LOG("Compiling vertex shader...\t");
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glcall(glShaderSource(vertex, 1, &vShaderCode, nullptr));
    glcall(glCompileShader(vertex));
    glcall(glGetShaderiv(vertex, GL_COMPILE_STATUS, &success));
    if (!success) {
        glcall(glGetShaderInfoLog(vertex, 512, nullptr, infolog));
        std::cout << "Failed to compile vertex shader!\n";
        std::cout << infolog << std::endl;
        glcall(glDeleteShader(vertex));
        return nullptr;
    }
    CONSOLE_LOG("DONE\n");

    CONSOLE_LOG("Compiling fragment shader...\t");
    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glcall(glShaderSource(fragment, 1, &fShaderCode, nullptr));
    glcall(glCompileShader(fragment));
    glcall(glGetShaderiv(fragment, GL_COMPILE_STATUS, &success));
    if (!success) {
        glcall(glGetShaderInfoLog(fragment, 512, nullptr, infolog));
        std::cout << "Failed to compile fragment shader! | " << fragmentFile
                  << std::endl;
        std::cout << infolog << std::endl;
        glcall(glDeleteShader(fragment));
        return nullptr;
    }
    CONSOLE_LOG("DONE\n");

    CONSOLE_LOG("Creating shader program...\t");
    unsigned int id = glCreateProgram();
    glcall(glAttachShader(id, vertex));
    glcall(glAttachShader(id, fragment));
    glcall(glLinkProgram(id));

    glcall(glGetProgramiv(id, GL_LINK_STATUS, &success));
    if (!success) {
        glcall(glGetProgramInfoLog(id, 512, nullptr, infolog));
        std::cout << "Failed to compile vertex shader! | " << vertexFile
                  << std::endl;
        std::cout << infolog << std::endl;
        glcall(glDeleteShader(vertex));
        glcall(glDeleteShader(fragment));
        glcall(glDeleteProgram(id));
        return nullptr;
    }
    CONSOLE_LOG("DONE\n");

    glcall(glDeleteShader(vertex));
    glcall(glDeleteShader(fragment));

    return new Shader(id);
}
