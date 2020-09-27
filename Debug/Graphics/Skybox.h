#pragma once

#include "Texture.h"
#include "Shader.h"
#include "../Mesh.h"
#include "../Camera.h"

class Skybox {
	Mesh* mesh;
	float* buffer;
	unsigned int index;
public:
	Shader* shader;
	Texture* texture;
	Skybox();
	void render(const Camera* cam);
};