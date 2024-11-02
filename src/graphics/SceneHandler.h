#pragma once

#include "../Camera.h"
#include "../Player.h"
#include "FrameBuffer.h"
#include "../objects.hpp"

class TerrarianHandler;
class LineBoxHandler;
class SkyboxHandler;

class SceneHandler {
    friend class MasterHandler;

    Player* plr;
    TerrarianHandler* terrarian;
    LineBoxHandler* lines;
    SkyboxHandler* skybox;
    FrameBuffer* fb;
    FrameBuffer* shadowBuff;
    glm::uvec2 window_size;

public:
    SceneHandler(glm::uvec2 window_size);

    void terminate();
    void updateAll(glm::uvec2 window_size);
    void updatePlayer(glm::uvec2 window_size);
    void updateChunks();

    void render(glm::uvec2 window_size);
};

class TerrarianHandler {
    friend class SceneHandler;

    tmine::Terrain terrain;

public:
    TerrarianHandler();
    void reloadChunks(Camera const* cam);
    void refreshRes();
    void terminate();
    void render(Camera const* cam, glm::uvec2 window_size);
    void renderShadows(
        Camera const* cam, FrameBuffer* shadowBuff, glm::uvec2 window_size
    );
};

class LineBoxHandler {
    friend class SceneHandler;

    tmine::LineBox* lineBatch;

public:
    LineBoxHandler();
    void terminate();
    void render(Camera const* cam, tmine::f32 aspect_ratio);
};

class SkyboxHandler {
    friend class SceneHandler;

    tmine::Skybox* skyboxes[5];
    int current;

public:
    SkyboxHandler();
    void terminate();
    void render(Camera const* cam, glm::uvec2 window_size);
};
