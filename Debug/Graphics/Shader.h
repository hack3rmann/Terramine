#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
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
};

extern Shader* load_shader(std::string vertexFile, std::string fragmentFile);

#endif // !SHADER_H
