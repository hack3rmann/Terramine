#include <math.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Mesh.h"
#include "Window.h"
#include "Camera.h"
#include "Player.h"
#include "defines.cpp"
#include "EventHandler.h"

#include "Voxels/voxel.h"
#include "Voxels/Chunk.h"
#include "Voxels/Chunks.h"
#include "Voxels/Terrarian.h"

#include "Graphics/Shader.h"
#include "Graphics/Skybox.h"
#include "Graphics/Texture.h"
#include "Graphics/LineBatch.h"
#include "Graphics/VoxelRenderer.h"

#include "GUI/GUI.h"

int main() {
	CONSOLE_LOG("Initializing window...\t");
	if (Window::init(WIDTH, HEIGHT, "Terramine") == -1) {
		system("pause");
		return -1;
	}
	CONSOLE_LOG("DONE\n");
	Events::init();

	Shader* GUIShader = load_shader("GUIVertex.glsl", "GUIFragment.glsl");
	if (GUIShader == nullptr) {
		std::cout << "Failed to load GUI shader!\n";

#		ifdef EXIT_ON_ERROR
		delete GUIShader;
		Window::terminate();
		Events::terminate();
		system("pause");
		return -1;
#		endif
	}

	Texture* sprite = load_texture("src/sprite.png");
	if (sprite == nullptr) {
		std::cerr << "Error: can't load sprite!\n";

#		ifdef EXIT_ON_ERROR
		delete GUIShader;
		delete sprite;
		Window::terminate();
		Events::terminate();
		system("pause");
		return -1;
#		endif
	}

	Terrarian* terra = new Terrarian();

	GUI* gui = new GUI();

	LineBatch* lineBatch = new LineBatch(4096);
	Skybox* skybox = new Skybox();

	CONSOLE_LOG("OpenGL predefines...\t")
	glcall(glClearColor(0.61f, 0.86f, 1.0f, 1.0f));
	glcall(glEnable(GL_DEPTH_TEST));
	glcall(glEnable(GL_CULL_FACE));
	glcall(glEnable(GL_BLEND));
	glcall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	glcall(glLineWidth(2.0f));
	CONSOLE_LOG("DONE\n");

	Player* plr = new Player(glfwGetTime(), -30.0f, vec3(0.0f));

	bool onceLoad = 1;

	while (!Window::isClosed()) {
		terra->reload();
		plr->update(terra->chunks, lineBatch);

		CONSOLE_LOG(plr->speed.y)CONSOLE_LOG("\r\t\t");

		if (Events::justPressed(GLFW_KEY_T)) Events::toggleCursor();
		if (Events::justPressed(GLFW_KEY_ESCAPE)) {
			Window::setShouldClose(true);
			break;
		}
		if (Events::justPressed(GLFW_KEY_R)) {
			terra->shader = load_shader("vertexShader.glsl", "fragmentShader.glsl");
			if (terra->shader == nullptr) CONSOLE_LOG("Failed to load terrarian shader!");

			lineBatch->shader = load_shader("linesVertex.glsl", "linesFragment.glsl");
			if (lineBatch->shader == nullptr) CONSOLE_LOG("Failed to load line shader!");

			GUIShader = load_shader("GUIVertex.glsl", "GUIFragment.glsl");
			if (GUIShader == nullptr) CONSOLE_LOG("Failed to load GUI shader!");

			skybox->shader = load_shader("SkyboxVertex.glsl", "SkyboxFragment.glsl");
			if (skybox->shader == nullptr) CONSOLE_LOG("Failed to load Skybox shader!");

			skybox->texture = load_texture("src/Skybox4.png");
		}

		glcall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		skybox->render(plr->cam);
		terra->render(plr->cam);
		lineBatch->render(plr->cam);

		GUIShader->use();
		GUIShader->uniformMatrix("projView", plr->cam->getProjection() * plr->cam->getView());
		sprite->bind();
		gui->render();

		onceLoad = 0;

		Window::swapBuffers();
		Events::pullEvents();
	}

	CONSOLE_LOG("Deleting pointers...\t");
	delete GUIShader;

	sprite->deleteTex();
	//delete sprite;

	delete terra;
	delete lineBatch;
	delete gui;
	CONSOLE_LOG("DONE\n")

	CONSOLE_LOG("Terminating systems...\t")
	Events::terminate();
	Window::terminate();
	CONSOLE_LOG("DONE\n");

	system("pause");
	return 0;
}

void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}
bool GLLogCall(const char* function, const char* file, int line) {
	while (GLenum error = glGetError()) {
		std::cout << "OpenGL error : " << error << ' ' << function << " : " << line << std::endl;
		return false;
	}
	return true;
}
