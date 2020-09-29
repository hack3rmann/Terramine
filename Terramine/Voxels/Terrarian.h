#pragma once

#include "Chunks.h"
#include "../Mesh.h"
#include "../Graphics/VoxelRenderer.h"
#include "../Graphics/Texture.h"
#include "../Graphics/Shader.h"
#include "../Player.h"

class Terrarian {
	Mesh** meshes;
	VoxelRenderer renderer;
	bool onceLoad;
	Texture* textureAtlas;
	vec3 toLightVec;
public:
	Shader* shader;
	Chunks* chunks;
	Terrarian();
	~Terrarian();
	void reload();
	void render(const Camera* cam);
};