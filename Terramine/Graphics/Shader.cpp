#include "Shader.h"

Shader::Shader(unsigned int id) : id(id) {

}
Shader::~Shader() {
	glDeleteProgram(id);
}
void Shader::use() {
	glUseProgram(id);
}

Shader* load_shader(std::string vertexFile, std::string fragmentFile) {
	std::string vertexCode, fragmentCode;
	std::ifstream vShaderFile, fShaderFile;

	vShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);

	try {
		vShaderFile.open("Graphics/Shaders/" + vertexFile);
		fShaderFile.open("Graphics/Shaders/" + fragmentFile);
		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	} catch (std::ifstream::failure&) {
		std::cerr << "Error : shader fiel not succesfully read\n";
		return nullptr;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	int success;
	char infolog[512];

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

	unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, nullptr);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, nullptr, infolog);
		std::cout << "Failed to compile fragment shader!\n";
		std::cout << infolog << std::endl;
		glDeleteShader(fragment);
		return nullptr;
	}

	unsigned int id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	glLinkProgram(id);

	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id, 512, nullptr, infolog);
		std::cout << "Failed to compile vertex shader!\n";
		std::cout << infolog << std::endl;
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		glDeleteProgram(id);
		return nullptr;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return new Shader(id);
}