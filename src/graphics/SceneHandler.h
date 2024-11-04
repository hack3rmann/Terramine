#pragma once

#include "../controls.hpp"
#include "../Player.h"
#include "FrameBuffer.h"
#include "../objects.hpp"

class TerrarianHandler;
class LineBoxHandler;
class SkyboxHandler;

class TerrarianHandler {
    friend class SceneHandler;

    tmine::Terrain terrain;

public:
    TerrarianHandler();
    void reloadChunks(tmine::Camera const* cam);
    void refreshRes();
    void render(tmine::Camera const* cam, glm::uvec2 window_size);
    void renderShadows(
        tmine::Camera const* cam, FrameBuffer* shadowBuff, glm::uvec2 window_size
    );
};

class LineBoxHandler {
    friend class SceneHandler;

    tmine::LineBox lineBatch;

public:
    LineBoxHandler();
    void render(tmine::Camera const* cam, tmine::f32 aspect_ratio);
};

class SkyboxHandler {
    friend class SceneHandler;

    std::vector<tmine::Skybox> skyboxes;
    int current;

public:
    SkyboxHandler();
    void render(tmine::Camera const* cam, glm::uvec2 window_size);
};

class SceneHandler {
    friend class MasterHandler;

    Player plr;
    TerrarianHandler terrarian;
    LineBoxHandler lines;
    SkyboxHandler skybox;
    FrameBuffer fb;
    FrameBuffer shadowBuff;
    glm::uvec2 window_size;

public:
    SceneHandler(glm::uvec2 window_size);

    void updateAll(glm::uvec2 window_size);
    void updatePlayer(glm::uvec2 window_size);
    void updateChunks();

    void render(glm::uvec2 window_size);
};
