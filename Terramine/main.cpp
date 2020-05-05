#include "indexBuffer.h"
#include "vertexBuffer.h"
#include "renderer.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>

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
		std::cout << "GLEW init error" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;
	
	{
		unsigned int shader = CreateShader();

		float vertex[] = {
//			  X      Y      Z       R     G     B
			-0.5f, -0.5f,  0.0f,   1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f,  0.0f,   0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f,  0.0f,   0.0f, 0.0f, 1.0f,
			-0.5f,  0.5f,  0.0f,   0.2f, 0.5f, 0.5f
		};
		unsigned int indeces[] = {
			0, 1, 2,
			2, 3, 0,
		};

		vBuffer vbo(vertex, 4 * 6 * sizeof(float));
		iBuffer ibo(indeces, 6);

		unsigned int positionID = glGetAttribLocation(shader, "vPosition");
		unsigned int colorID = glGetAttribLocation(shader, "vColor");

		glcall(glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0));
		glcall(glVertexAttribPointer(colorID, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void*)(3 * sizeof(float))));
		glcall(glEnableVertexAttribArray(positionID));
		glcall(glEnableVertexAttribArray(colorID));
		glcall(glUseProgram(shader));

		while (!glfwWindowShouldClose(window)) {
			glClear(GL_COLOR_BUFFER_BIT);

			glcall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

			// Swap front and back buffers
			glfwSwapBuffers(window);

			// Poll for and process events
			glfwPollEvents();
		}
	}

	glfwTerminate();
	return 0;
}
