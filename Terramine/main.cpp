#include "defines.cpp"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Mesh.h"
#include "Window.h"
#include "Camera.h"
#include "Graphics/Shader.h"
#include "Graphics/VoxelRenderer.h"
#include "Graphics/Texture.h"
#include "Graphics/LineBatch.h"
#include "Voxels/voxel.h"
#include "Voxels/Chunk.h"
#include "Voxels/Chunks.h"
#include "GUI/GUI.h"
#include "EventHandler.h"
#include "Player.h"
#include <math.h>
#include <string>

#pragma warning(disable : 4996)

int main() {
	CONSOLE_LOG("Initializing window...\t");
	if (Window::init(WIDTH, HEIGHT, "Terramine") == -1) {
		system("pause");
		return -1;
	}
	CONSOLE_LOG("DONE\n");
	Events::init();
	
	Shader* shader = load_shader("vertexShader.glsl", "fragmentShader.glsl");
	if (shader == nullptr) {
		std::cout << "Failed to load main shader!\n";

#		ifdef EXIT_ON_ERROR
			delete shader;
			Window::terminate();
			Events::terminate();
			system("pause");
			return -1;
#		endif
	}

	Shader* linesShader = load_shader("linesVertex.glsl", "linesFragment.glsl");
	if (linesShader == nullptr) {
		std::cout << "Failed to load lines shader!\n";

#		ifdef EXIT_ON_ERROR
			delete linesShader;
			Window::terminate();
			Events::terminate();
			system("pause");
			return -1;
#		endif
	}

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
		delete shader;
		delete linesShader;
		delete GUIShader;
		delete sprite;
		Window::terminate();
		Events::terminate();
		system("pause");
		return -1;
#		endif
	}

	Texture* texture = load_texture("src/textureAtlas3.png");
	if (texture == nullptr) {
		std::cerr << "Error: can't load texture!\n";

#		ifdef EXIT_ON_ERROR
			delete shader;
			delete texture;
			Window::terminate();
			Events::terminate();
			system("pause");
			return -1;
#		endif
	}

	Chunks* chunks = new Chunks(8, 8, 8);

	CONSOLE_LOG("Creating 2d meshes array...\t");
	Mesh** meshes = new Mesh*[chunks->volume];
	for (unsigned long long i = 0; i < chunks->volume; i++)
		meshes[i] = nullptr;
	CONSOLE_LOG("DONE\n");

	GUI* gui = new GUI();
	gui->addSprite(0.2f, 0.2f, 0.8f, 0.8f, sprite);
	gui->addSprite(-0.2f, -0.2f, 0.8f, 0.8f, sprite);

	VoxelRenderer renderer(1024 * 1024 * 4);
	LineBatch* lineBatch = new LineBatch(4096);

	CONSOLE_LOG("OpenGL predefines...\t")
	glcall(glClearColor(0.61f, 0.86f, 1.0f, 1.0f));
	glcall(glEnable(GL_DEPTH_TEST));
	glcall(glEnable(GL_CULL_FACE));
	glcall(glEnable(GL_BLEND));
	glcall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	glcall(glLineWidth(2.0f));
	CONSOLE_LOG("DONE\n");

	Player* plr = new Player(glfwGetTime(), -30.0f, vec3(0.0f));

	bool onceLoad = true;

	while (!Window::isClosed()) {
		int percentage = 0;
		Chunk* closes[27];
		for (unsigned long long i = 0; i < chunks->volume; i++) {
			Chunk* chunk = chunks->chunks[i];
			if (!chunk->modified)
				continue;
			chunk->modified = false;
			if (meshes[i] != nullptr)
				delete meshes[i];

			if (percentage != (int)((float)i / (float)(chunks->volume - 1) * 100.0f) || i == 0) {
				if (onceLoad)
				CONSOLE_LOG("Reloading chunks...\t");
				percentage = (int)((float)i / (float)(chunks->volume - 1) * 100.0f);
				if (onceLoad)
				CONSOLE_LOG(percentage)if (onceLoad)CONSOLE_LOG("%\r")
			}
			if (i == chunks->volume - 1) {
				if (onceLoad)
				CONSOLE_LOG("\n");
			}

			for (int i = 0; i < 27; i++)
				closes[i] = nullptr;
			for (unsigned long long j = 0; j < chunks->volume; j++) {
				Chunk* other = chunks->chunks[j];

				int ox = other->x - chunk->x;
				int oy = other->y - chunk->y;
				int oz = other->z - chunk->z;

				if (abs(ox) > 1 || abs(oy) > 1 || abs(oz) > 1)
					continue;

				ox += 1;
				oy += 1;
				oz += 1;
				closes[(oy * 3 + oz) * 3 + ox] = other;
			}
			Mesh* mesh = renderer.render(chunk, (const Chunk**)closes, true);
			meshes[i] = mesh;
		}

		plr->update(chunks);

		if (Events::justPressed(GLFW_KEY_T)) Events::toggleCursor();
		if (Events::justPressed(GLFW_KEY_ESCAPE)) {
			Window::setShouldClose(true);
			break;
		}

		CONSOLE_LOG(plr->speed.y)CONSOLE_LOG("\r\t\t");
		
		if (Events::justPressed(GLFW_KEY_R)) {
			shader = load_shader("vertexShader.glsl", "fragmentShader.glsl");
			if (shader == nullptr) CONSOLE_LOG("Failed to load main shader!");

			linesShader = load_shader("linesVertex.glsl", "linesFragment.glsl");
			if (linesShader == nullptr) CONSOLE_LOG("Failed to load line shader!");

			GUIShader = load_shader("GUIVertex.glsl", "GUIFragment.glsl");
			if (GUIShader == nullptr) CONSOLE_LOG("Failed to load GUI shader!");
		}

		/* Ray casting */
		{
			vec3 end;
			vec3 norm;
			vec3 iend;
			voxel* vox = chunks->rayCast(plr->cam->position, plr->cam->frontCam, 10.0f, end, norm, iend);
			if (vox != nullptr) {
				CONSOLE_LOG(vox->name)CONSOLE_LOG("                                 \r");
				lineBatch->box(iend.x + 0.5f, iend.y + 0.5f, iend.z + 0.5f, 1.001f, 1.001f, 1.001f, 60.0f / 255.0f, 60.0f / 255.0f, 60.0f / 255.0f, 1.0f);

				if (Events::justClicked(GLFW_MOUSE_BUTTON_1)) {
					chunks->set((int)iend.x, (int)iend.y, (int)iend.z, 0);
				}
				if (Events::justClicked(GLFW_MOUSE_BUTTON_2)) {
					chunks->set((int)iend.x + (int)norm.x, (int)iend.y + (int)norm.y, (int)iend.z + (int)norm.z, plr->currentBlock);
				}
			} else {
				CONSOLE_LOG("                                     \r");
			}
		}

		glcall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		linesShader->use();
		linesShader->uniformMatrix("projView", plr->cam->getProjection() * plr->cam->getView());
		lineBatch->render();

		shader->use();
		shader->uniformMatrix("projView", plr->cam->getProjection() * plr->cam->getView());
		shader->uniformVec2u("resolution", vec2(Window::width, Window::height));
		texture->bind();
		mat4 model(1.0f);
		for (unsigned long long i = 0; i < chunks->volume; i++) {
			Chunk* chunk = chunks->chunks[i];
			Mesh* mesh = meshes[i];
			if (mesh == nullptr)
				continue;
			model = glm::translate(mat4(1.0f), vec3(chunk->x * CHUNK_W + 0.5f, chunk->y * CHUNK_H + 0.5f, chunk->z * CHUNK_D + 0.5f));
			shader->uniformMatrix("model", model);
			mesh->draw(GL_TRIANGLES);
		}

		GUIShader->use();
		GUIShader->uniformMatrix("projView", plr->cam->getProjection() * plr->cam->getView());
		sprite->bind();
		gui->render();

		onceLoad = 0;

		Window::swapBuffers();
		Events::pullEvents();
	}

	CONSOLE_LOG("Deleting pointers...\t");
	delete shader;
	delete linesShader;
	delete GUIShader;

	delete texture;
	delete sprite;

	delete chunks;
	delete lineBatch;
	delete gui;
	CONSOLE_LOG("DONE\n")

	CONSOLE_LOG("Terminating systems...\t")
	Events::terminate();
	Window::terminate();
	CONSOLE_LOG("DONE\n")

	system("pause");
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
