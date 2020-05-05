#include "renderer.h"

std::string parseShader(const std::string& name) {
	std::ifstream file;
	file.open("Shaders/" + name);
	std::string shader;

	if (file.is_open()) {
		char ch;
		while (!file.eof()) {
			file.get(ch);
			shader.push_back(ch);
		}
		file.close();
	} else {
		std::cout << "File error" << std::endl;
	}

	return shader;
}
unsigned int CompileShader(unsigned int type, const std::string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int res;
	glGetShaderiv(id, GL_COMPILE_STATUS, &res);
	if (!res) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!\n";
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}
int CreateShader() {
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, parseShader("vertexShader.glsl"));
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, parseShader("fragmentShader.glsl"));

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}
void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}
bool GLLogCall(const char* function, const char* file, int line) {
	while (GLenum error = glGetError()) {
		std::cout << "OpenGL eeror : " << error << ' ' << function << " : " << line << std::endl;
		return false;
	}
	return true;
}
