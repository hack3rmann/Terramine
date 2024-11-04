#pragma once

#include <glad/gl.h>

#include "../GUI/GUIHandler.h"
#include "SceneHandler.h"
#include "../window.hpp"

class MasterHandler {
public:
    SceneHandler sceneHandler;
    GUIHandler gui;

    explicit MasterHandler(tmine::Window* window);
    
    void render(glm::uvec2 window_size);
    void updateAll(glm::uvec2 window_size);
};
