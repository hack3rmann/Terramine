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
	mat4 rotation;

	Camera* lightVision;
public:
	Shader* shader;
	Chunks* chunks;
	Chunk* chunksRend[125];

	Terrarian(const char* textureAtlas);
	~Terrarian();
	void reload(const Camera* cam);
	void refreshShader();
	void refreshTextures();
	void render(const Camera* cam, FrameBuffer* shadowBuff);
	void renderTerrarian(const Camera* cam);
	void refreshLightVision(const Camera* cam);
	void renderTerrarianIsometric(const Camera* cam);
};