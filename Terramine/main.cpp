#include "Window.h"
#include "Graphics/Shader.h"
#include "EventHandler.h"

#define BUFFER_OFFSET(i) ((char*)NULL + (i))
#define ASSERT(x) if (!(x)) __debugbreak();
#define glcall(x)                               \
     GLClearError();                            \
     x;                                         \
     ASSERT(GLLogCall(#x, __FILE__, __LINE__));

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

#define width 640
#define height 480

float vertices[] = {
//    X      Y     Z
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
};

int main() {
	Window::init(width, height, "Terramine");
	Events::init();

	Shader* shader = load_shader("vertexShader.glsl", "fragmentShader.glsl");
	if (shader == nullptr) {
		std::cout << "Failed to load shader\n";
		Window::terminate();
		return -1;
	}

	unsigned int vao, vbo;
	glcall(glGenVertexArrays(1, &vao));
	glcall(glGenBuffers(1, &vbo));

	glcall(glBindVertexArray(vao));
	glcall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	glcall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

	glcall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0 * sizeof(float))));
	glcall(glEnableVertexAttribArray(0));

	glcall(glBindVertexArray(NULL));

	glcall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
	while (!Window::isClosed()) {
		Events::pullEvents();
		if (Events::justPressed(GLFW_KEY_ESCAPE)) {
			Window::setShouldClose(true);
		}

		if (Events::justPressed(GLFW_KEY_R)) { glcall(glClearColor(1.0f, 0.0f, 0.0f, 1.0f)); }
		if (Events::justPressed(GLFW_KEY_G)) { glcall(glClearColor(0.0f, 1.0f, 0.0f, 1.0f)); }
		if (Events::justPressed(GLFW_KEY_B)) { glcall(glClearColor(0.0f, 0.0f, 1.0f, 1.0f)); }

		shader->use();
		glcall(glBindVertexArray(vao));

		glcall(glClear(GL_COLOR_BUFFER_BIT));

		glcall(glDrawArrays(GL_TRIANGLES, 0, 3));

		Window::swapBuffers();
	}

	Window::terminate();
	return 0;
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
