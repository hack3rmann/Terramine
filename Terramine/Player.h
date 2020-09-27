#pragma once

#include "Camera.h"
#include <glm/glm.hpp>
#include "EventHandler.h"
#include "Voxels/Chunks.h"
#include "Graphics/LineBatch.h"

using namespace glm;

class Player {
	float lastTime;
	float currTime;
	float dTime;
	float gravity;
	float camX;
	float camY;
public:
	LineBatch* lineBatch;
	Camera* cam;
	vec3 speed;
	int currentBlock;
	bool isSpeedUp;

	Player();
	Player(float lastTime, float gravity, vec3 speed);
	void update(Chunks* chunks, LineBatch* lineBatch);
	void updateTime();
};