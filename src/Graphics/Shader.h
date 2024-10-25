#pragma once

#include <exception>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>

class Shader {
public:
    unsigned int id;

    Shader(unsigned int id);
    ~Shader();

    void use();
    void uniformMatrix(char const* name, glm::mat4 matrix);
    void uniformVec2u(char const* name, glm::vec2 vec);
    void uniform3f(char const* name, glm::vec3 vec);
    void uniform1i(char const* name, int num);
};

extern Shader* load_shader(
    std::string const vertexFile, std::string const fragmentFile
);
