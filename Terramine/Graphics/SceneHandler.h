#pragma once

#include "ShadowFrameBuffer.h"
#include "../Mesh.h"
#include "../Player.h"
#include "../Camera.h"
#include "../Voxels/Terrarian.h"
#include "Skybox.h"
#include "FrameBuffer.h"

class TerrarianHandler;
class LineBatchHandler;
class SkyboxHandler;

class SceneHandler {
	friend class MasterHandler;

	Player* plr;
	TerrarianHandler* terrarian;
	LineBatchHandler* lines;
	SkyboxHandler* skybox;
	FrameBuffer* fb;
	//ShadowFB* shadowFB;
public:
	SceneHandler();

	void terminate();
	void updateAll();
	void updatePlayer();
	void updateChunks();

	void render();
};

class TerrarianHandler {
	friend class SceneHandler;

	Terrarian* terra;
public:
	TerrarianHandler();
	void reloadChunks();
	void refreshRes();
	void terminate();
	void render(const Camera* cam);
};

class LineBatchHandler {
	friend class SceneHandler;

	LineBatch* lineBatch;
public:
	LineBatchHandler();
	void terminate();
	void render(const Camera* cam);
};

class SkyboxHandler {
	friend class SceneHandler;

	Skybox* skyboxes[5];
	int current;
public:
	SkyboxHandler();
	void terminate();
	void render(const Camera* cam);
};