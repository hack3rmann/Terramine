#pragma once

#include "Texture.h"
#include "Shader.h"
#include "../Mesh.h"
#include "../Camera.h"

class Skybox {
	friend class SkyboxHandler;

	Mesh* mesh;
	float* buffer;
	unsigned int index;
public:
	Shader* shader;
	Texture* texture;
	Skybox(const char* name);
	void render(const Camera* cam);
};