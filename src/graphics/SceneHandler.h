#pragma once

#include "../Camera.h"
#include "../Player.h"
#include "../Voxels/Terrarian.h"
#include "FrameBuffer.h"
#include "../objects.hpp"

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
    FrameBuffer* shadowBuff;

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
    void reloadChunks(Camera const* cam);
    void refreshRes();
    void terminate();
    void render(Camera const* cam);
    void renderShadows(Camera const* cam, FrameBuffer* shadowBuff);
};

class LineBatchHandler {
    friend class SceneHandler;

    LineBatch* lineBatch;

public:
    LineBatchHandler();
    void terminate();
    void render(Camera const* cam);
};

class SkyboxHandler {
    friend class SceneHandler;

    tmine::Skybox* skyboxes[5];
    int current;

public:
    SkyboxHandler();
    void terminate();
    void render(Camera const* cam);
};
