#pragma once

#include "Chunks.h"
#include "../Mesh.h"
#include "../Graphics/VoxelRenderer.h"
#include "../Graphics/Texture.h"
#include "../Graphics/Shader.h"
#include "../Graphics/FrameBuffer.h"
#include "../Player.h"

class Terrarian {
	friend class TerrarianHandler;

	Mesh** meshes;
	VoxelRenderer renderer;
	bool onceLoad;
	Texture* textureAtlas;
	Texture* normalAtlas;
	vec3 toLightVec;

public:
	Shader* shader;
	Chunks* chunks;
	Terrarian(const char* textureAtlas);
	~Terrarian();
	void reload();
	void refreshShader();
	void refreshTextures();
	void render(const Camera* cam);
};