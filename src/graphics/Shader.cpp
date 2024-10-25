#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

Shader::Shader(unsigned int id)
    : id(id) {}

Shader::~Shader() { glDeleteProgram(id); }

void Shader::use() { glUseProgram(id); }

void Shader::uniformMatrix(char const* name, glm::mat4 matrix) {
    GLuint transformLoc = glGetUniformLocation(id, name);
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matrix)
    );
}

void Shader::uniformVec2u(char const* name, glm::vec2 vec) {
    GLuint loc = glGetUniformLocation(id, name);
    glUniform2f(loc, vec.x, vec.y);
}

void Shader::uniform3f(char const* name, glm::vec3 vec) {
    GLuint loc = glGetUniformLocation(id, name);
    glUniform3f(loc, vec.x, vec.y, vec.z);
}

void Shader::uniform1i(char const* name, int num) {
    GLuint loc = glGetUniformLocation(id, name);
    glUniform1i(loc, num);
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

    fprintf(stderr, "Compiling vertex shader...\t");
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, nullptr, infolog);
        std::cout << "Failed to compile vertex shader!\n";
        std::cout << infolog << std::endl;
        glDeleteShader(vertex);
        return nullptr;
    }
    fprintf(stderr, "DONE\n");

    fprintf(stderr, "Compiling fragment shader...\t");
    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, nullptr, infolog);
        std::cout << "Failed to compile fragment shader! | " << fragmentFile
                  << std::endl;
        std::cout << infolog << std::endl;
        glDeleteShader(fragment);
        return nullptr;
    }
    fprintf(stderr, "DONE\n");

    fprintf(stderr, "Creating shader program...\t");
    unsigned int id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);

    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id, 512, nullptr, infolog);
        std::cout << "Failed to compile vertex shader! | " << vertexFile
                  << std::endl;
        std::cout << infolog << std::endl;
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteProgram(id);
        return nullptr;
    }
    fprintf(stderr, "DONE\n");

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return new Shader(id);
}
