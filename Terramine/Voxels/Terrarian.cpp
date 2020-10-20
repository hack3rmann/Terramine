#include "Terrarian.h"
#include "../EventHandler.h"
#include "Chunk.h"
#include "../defines.cpp"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#define RENDER_DISTANCE 1
#define RENDER_VOLUME 27//RENDER_DISTANCE * RENDER_DISTANCE * RENDER_DISTANCE

mat4 rotation(1.0f);

Terrarian::Terrarian(const char* textureAtlas) : renderer(1024 * 1024 * 3) {
	onceLoad = 1;

	/* Loading textures */
	this->textureAtlas = load_texture("src/textureAtlas4.png");
	if (this->textureAtlas == nullptr) {
		CONSOLE_LOG("Can not load texture in ")CONSOLE_LOG(__FILE__)CONSOLE_LOG(", ")CONSOLE_LOG(__LINE__);
		delete this->textureAtlas;
	}
	this->normalAtlas = load_texture("src/normalAtlas3.png");
	if (normalAtlas == nullptr) {
		CONSOLE_LOG("Can not load texture in ")CONSOLE_LOG(__FILE__)CONSOLE_LOG(", ")CONSOLE_LOG(__LINE__);
		delete normalAtlas;
	}

	/* Loading shader */
	shader = load_shader("vertexShader.glsl", "fragmentShader.glsl");
	if (shader == nullptr) {
		CONSOLE_LOG("Can not load shader in ")CONSOLE_LOG(__FILE__)CONSOLE_LOG(", ")CONSOLE_LOG(__LINE__);
		delete textureAtlas;
		delete shader;
	}

	chunks = new Chunks(16, 16, 16);

	/* Light init */
	lightVision = new Camera(vec3(0.0f), 90.0f);
	rotation = mat4(1.0f);
	rotation = rotate(rotation, radians(-30.0f), vec3(1.0f, 0.0f, 0.0f));
	toLightVec = vec3(rotation * vec4(vec3(0.0f, 0.0f, 0.0f), 1.0f));

	CONSOLE_LOG("Creating 2d meshes array...\t");
	meshes = new Mesh*[chunks->volume];
	for (unsigned long long i = 0; i < chunks->volume; i++)
		meshes[i] = nullptr;
	CONSOLE_LOG("DONE\n");

}
Terrarian::~Terrarian() {
	delete shader;
	textureAtlas->deleteTex();
	delete chunks;
}
void Terrarian::reload(const Camera* cam) {
	int percentage = 0;

	Chunk* closes[27];

	float w = chunks->w,
		  h = chunks->h,
		  d = chunks->d;

	int chunk_pos_x = (int)(cam->position.x) / CHUNK_W;
	int chunk_pos_y = (int)(cam->position.y) / CHUNK_H;
	int chunk_pos_z = (int)(cam->position.z) / CHUNK_D;
	
	int index = 0;
	for (int y = -RENDER_DISTANCE; y <= RENDER_DISTANCE; y++) {
		for (int x = -RENDER_DISTANCE; x <= RENDER_DISTANCE; x++) {
			for (int z = -RENDER_DISTANCE; z <= RENDER_DISTANCE; z++, index++) {
				chunksRend[index] = chunks->getChunk(chunk_pos_x + x, chunk_pos_y + y, chunk_pos_z + z);
			}
		}
	}

	for (unsigned int i = 0; i < RENDER_VOLUME; i++) {
		Chunk* chunk = chunksRend[i];
		if (chunk == nullptr)
			continue;
		if (!chunk->modified)
			continue;
		chunk->modified = false;
		if (meshes[i] != nullptr)
			delete meshes[i];

		if (percentage != (int)((float)i / (float)(RENDER_VOLUME - 1) * 100.0f) || i == 0) {
			percentage = (int)((float)i / (float)(RENDER_VOLUME - 1) * 100.0f);
			if (onceLoad) {
				CONSOLE_LOG("Reloading chunks...\t")
				CONSOLE_LOG(percentage)
				CONSOLE_LOG("%\r")
			}
		}
		if (i == RENDER_VOLUME - 1) {
			if (onceLoad)
				CONSOLE_LOG("\n");
		}

		for (int i = 0; i < 27; i++)
			closes[i] = nullptr;
		for (unsigned long long j = 0; j < 27; j++) {
			Chunk* other = chunksRend[j];
			if (other == nullptr)
				continue;

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
void Terrarian::renderTerrarian(const Camera* cam) {
	shader->use();

	/* Light rotation */
	refreshLightVision(cam);
	if (Events::isPressed(GLFW_KEY_P))
		rotation = rotate(rotation, radians(0.1f), vec3(1.0f, 0.0f, 0.0f));
	if (Events::isPressed(GLFW_KEY_O))
		rotation = rotate(rotation, radians(0.1f), vec3(0.0f, 1.0f, 0.0f));
	if (Events::isPressed(GLFW_KEY_I))
		rotation = rotate(rotation, radians(0.1f), vec3(0.0f, 0.0f, 1.0f));

	/* Shader uniforms */
	shader->uniformMatrix("proj", lightVision->getOrtho());
	shader->uniformMatrix("view", lightVision->getView());
	shader->uniformVec2u("resolution", vec2(Window::width, Window::height));
	shader->uniform3f("toLightVec", toLightVec);
	shader->uniform3f("lightColor", vec3(0.96f, 0.24f, 0.0f));
	shader->uniform1i("u_Texture0", 0);
	shader->uniform1i("u_Texture1", 1);

	/* Binding textures */
	glcall(glActiveTexture(GL_TEXTURE0));
	textureAtlas->bind();
	glcall(glActiveTexture(GL_TEXTURE1));
	normalAtlas->bind();

	/* Drawing */
	mat4 model(1.0f);
	for (unsigned long long i = 0; i < RENDER_VOLUME; i++) {
		Chunk* chunk = chunksRend[i];
		if (chunk == nullptr)
			continue;
		Mesh* mesh = meshes[i];
		if (mesh == nullptr)
			continue;
		model = glm::translate(mat4(1.0f), vec3(chunk->x * CHUNK_W + 0.5f, chunk->y * CHUNK_H + 0.5f, chunk->z * CHUNK_D + 0.5f));
		shader->uniformMatrix("model", model);
		mesh->draw(GL_TRIANGLES);
	}

	/* Return to TEXTURE0 */
	glcall(glActiveTexture(GL_TEXTURE0));
}
void Terrarian::render(const Camera* cam, FrameBuffer* shadowBuff) {
	shader->use();

	/* Shader uniforms */
	shader->uniformMatrix("proj", cam->getProjection());
	shader->uniformMatrix("view", cam->getView());
	shader->uniformMatrix("toLightSpace", lightVision->getView() * lightVision->getOrtho());
	shader->uniformVec2u("resolution", vec2(Window::width, Window::height));
	shader->uniform3f("toLightVec", toLightVec);
	shader->uniform3f("lightColor", vec3(0.96f, 0.24f, 0.0f));
	shader->uniform1i("u_Texture0", 0);
	shader->uniform1i("u_Texture1", 1);
	shader->uniform1i("u_Texture2", 2);

	/* Binding textures */
	glcall(glActiveTexture(GL_TEXTURE0));
	textureAtlas->bind();
	glcall(glActiveTexture(GL_TEXTURE1));
	normalAtlas->bind();
	glcall(glActiveTexture(GL_TEXTURE2));
	shadowBuff->bindDepthTex();

	/* Drawing */
	mat4 model(1.0f);
	for (unsigned long long i = 0; i < RENDER_VOLUME; i++) {
		Chunk* chunk = chunksRend[i];
		if (chunk == nullptr)
			continue;
		Mesh* mesh = meshes[i];
		if (mesh == nullptr)
			continue;
		model = glm::translate(mat4(1.0f), vec3(chunk->x * CHUNK_W + 0.5f, chunk->y * CHUNK_H + 0.5f, chunk->z * CHUNK_D + 0.5f));
		shader->uniformMatrix("model", model);
		mesh->draw(GL_TRIANGLES);
	}

	/* Return to TEXTURE0 */
	glcall(glActiveTexture(GL_TEXTURE0));
}
void Terrarian::renderTerrarianIsometric(const Camera* cam) {
	shader->use();

	/* Shader uniforms */
	shader->uniformMatrix("proj", cam->getOrtho());
	shader->uniformMatrix("view", cam->getView());
	shader->uniformVec2u("resolution", vec2(Window::width, Window::height));
	shader->uniform3f("toLightVec", toLightVec);
	shader->uniform3f("lightColor", vec3(0.96f, 0.24f, 0.0f));
	shader->uniform1i("u_Texture0", 0);
	shader->uniform1i("u_Texture1", 1);

	/* Binding textures */
	glcall(glActiveTexture(GL_TEXTURE0));
	textureAtlas->bind();
	glcall(glActiveTexture(GL_TEXTURE1));
	normalAtlas->bind();

	/* Drawing */
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

	/* Return to TEXTURE0 */
	glcall(glActiveTexture(GL_TEXTURE0));
}
void Terrarian::refreshShader() {
	shader = load_shader("vertexShader.glsl", "fragmentShader.glsl");
}
void Terrarian::refreshTextures() {
	textureAtlas = load_texture("src/textureAtlas4.png");
	normalAtlas = load_texture("src/normalAtlas3.png");
}
void Terrarian::refreshLightVision(const Camera* cam) {
	lightVision->position = cam->position;
	lightVision->position.y += 10.0f;
	lightVision->rotation = rotation;/*
	lightVision->frontCam = -toLightVec;
	lightVision->right = vec3(-toLightVec.y, toLightVec.x, toLightVec.z);
	lightVision->up = cross(lightVision->right, lightVision->frontCam);*/
	lightVision->frontCam = vec3(0.0f, 0.0f, -1.0f);
	lightVision->right = vec3(1.0f, 0.0f, 0.0f);
	lightVision->up = vec3(0.0f, 1.0f, 0.0f);
	lightVision->updateVectors();
}