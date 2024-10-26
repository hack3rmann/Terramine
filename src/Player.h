#pragma once

#include <glm/glm.hpp>

#include "Camera.h"
#include "objects.hpp"
#include "Voxels/Chunks.h"

using namespace glm;

class Player {
    float lastTime;
    float currTime;
    float dTime;
    float gravity;
    float camX;
    float camY;

public:
    tmine::LineBox* lineBatch;
    Camera* cam;
    vec3 speed;
    int currentBlock;
    bool isSpeedUp;

    Player();
    Player(float lastTime, float gravity, vec3 speed);
    void update(Chunks* chunks, tmine::LineBox* lineBatch);
    void updateTime();
};
