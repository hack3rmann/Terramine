#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>

#define BUFFER_OFFSET(i) ((char*)NULL + (i))
#define DEBUG

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
static unsigned int CompileShader(unsigned int type, const std::string& source) {
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
static int CreateShader() {
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

int main() {
	GLFWwindow* window;
	
	// Initialize the library
	if (!glfwInit())
		return -1;

	// Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(640, 480, "Terramine", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	// Make the window's context current
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
		std::cout << "GLEW err" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;
	
	unsigned int shader = CreateShader();

	float vertex[] = {
//		  X      Y      Z
		-0.5f, -0.5f,  0.0f,
		 0.5f, -0.5f,  0.0f,
		 0.5f,  0.5f,  0.0f,
		-0.5f,  0.5f,  0.0f
	};
	float color[] = {
//		 R     G     B     A
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f
	};
	unsigned int indeces[] = {
		0, 1, 2,
		2, 3, 0
	};

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	glBufferData(GL_ARRAY_BUFFER, 4 * 7 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 7 * sizeof(unsigned int), indeces, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * 3 * sizeof(float), vertex);
	glBufferSubData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(float), 4 * 4 * sizeof(float), color);

	unsigned int positionID = glGetAttribLocation(shader, "vPosition");
	unsigned int colorID = glGetAttribLocation(shader, "vColor");

	glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, (const void*)(4 * 3 * sizeof(float)));
	glUseProgram(shader);
	glEnableVertexAttribArray(positionID);
	glEnableVertexAttribArray(colorID);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}