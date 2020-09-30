#include "Terrarian.h"
#include "../EventHandler.h"
#include "Chunk.h"
#include "../defines.cpp"
#include <iostream>

Terrarian::Terrarian() : renderer(1024 * 1024 * 4) {
	onceLoad = 1;
	textureAtlas = load_texture("src/textureAtlas3.png");
	if (textureAtlas == nullptr) {
		CONSOLE_LOG("Can not load texture in ")CONSOLE_LOG(__FILE__)CONSOLE_LOG(", ")CONSOLE_LOG(__LINE__);
		delete textureAtlas;
	}
	shader = load_shader("vertexShader.glsl", "fragmentShader.glsl");
	//shader = load_shader("vertexShadow.glsl", "fragmentShadow.glsl");
	if (shader == nullptr) {
		CONSOLE_LOG("Can not load shader in ")CONSOLE_LOG(__FILE__)CONSOLE_LOG(", ")CONSOLE_LOG(__LINE__);
		delete textureAtlas;
		delete shader;
	}

	chunks = new Chunks(32, 8, 32);

	toLightVec = vec3(-0.2f, 0.5f, -1.0f);

	CONSOLE_LOG("Creating 2d meshes array...\t");
	meshes = new Mesh*[chunks->volume];
	for (unsigned long long i = 0; i < chunks->volume; i++)
		meshes[i] = nullptr;
	CONSOLE_LOG("DONE\n");
}
Terrarian::~Terrarian() {
	delete shader;
	textureAtlas->deleteTex();
	//delete textureAtlas;
	delete chunks;
}
void Terrarian::reload() {
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
			percentage = (int)((float)i / (float)(chunks->volume - 1) * 100.0f);
			if (onceLoad) {
				CONSOLE_LOG("Reloading chunks...\t")
				CONSOLE_LOG(percentage)
				CONSOLE_LOG("%\r")
			}
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
	onceLoad = 0;
}
void Terrarian::render(const Camera* cam) {
	shader->use();
	shader->uniformMatrix("proj", cam->getProjection());
	shader->uniformMatrix("view", cam->getView());
	shader->uniformVec2u("resolution", vec2(Window::width, Window::height));
	shader->uniform3f("toLightVec", toLightVec);
	shader->uniform3f("lightColor", vec3(0.96f, 0.24f, 0.0f));
	textureAtlas->bind();
	mat4 model(1.0f);
	glcall(glEnable(GL_DEPTH_TEST));
	glcall(glEnable(GL_CULL_FACE));
	for (unsigned long long i = 0; i < chunks->volume; i++) {
		Chunk* chunk = chunks->chunks[i];
		Mesh* mesh = meshes[i];
		if (mesh == nullptr)
			continue;
		model = glm::translate(mat4(1.0f), vec3(chunk->x * CHUNK_W + 0.5f, chunk->y * CHUNK_H + 0.5f, chunk->z * CHUNK_D + 0.5f));
		shader->uniformMatrix("model", model);
		//ShadowShader->uniformMatrix("mvp", cam->getProjection() * cam->getView() * model);
		mesh->draw(GL_TRIANGLES);
	}
	glcall(glDisable(GL_DEPTH_TEST));
	glcall(glDisable(GL_CULL_FACE));
}