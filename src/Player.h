#pragma once

#include <glm/glm.hpp>

#include "controls.hpp"
#include "objects.hpp"
#include "physics.hpp"

using namespace glm;

class Player {
    float lastTime;
    float currTime;
    float dTime;
    float gravity;
    float camX;
    float camY;

public:
    tmine::Camera cam;
    vec3 speed;
    int currentBlock;
    bool isSpeedUp;

    Player();
    Player(float lastTime, float gravity, vec3 speed);
    void update(
        tmine::RefMut<tmine::BoxCollider> player_collider,
        tmine::RefMut<tmine::Terrain> terrain,
        tmine::RefMut<tmine::LineBox> lineBatch, glm::uvec2 window_size
    );
    void updateTime();
};
