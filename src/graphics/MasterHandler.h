#pragma once

#include <glad/gl.h>

#include "../GUI/GUIHandler.h"
#include "SceneHandler.h"
#include "../window.hpp"

class MasterHandler {
public:
    static SceneHandler* sceneHandler;
    static GUIHandler* gui;

    static void init(tmine::Window* window);
    static void render(glm::uvec2 window_size);
    static void terminate();
    static void updateAll(glm::uvec2 window_size);
};
