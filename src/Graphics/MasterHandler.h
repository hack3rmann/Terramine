#pragma once

#include <glad/gl.h>

#include "../GUI/GUIHandler.h"
#include "SceneHandler.h"

class MasterHandler {
public:
    static SceneHandler* sceneHandler;
    static GUIHandler* gui;

    static void init();
    static void render();
    static void terminate();
    static void updateAll();
};
