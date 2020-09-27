#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <exception>
#include <sstream>
#include <fstream>

class Shader {
public:
	unsigned int id;

	Shader(unsigned int id);
	~Shader();

	void use();
	void uniformMatrix(const char* name, glm::mat4 matrix);
	void uniformVec2u(const char* name, glm::vec2 vec);
	void uniform3f(const char* name, glm::vec3 vec);
	void uniform1i(const char* name, int num);
};

extern Shader* load_shader(const std::string vertexFile, const std::string fragmentFile);

#endif // !SHADER_H
