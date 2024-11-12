#pragma once

#include <glad/gl.h>

#include "SceneHandler.h"
#include "../gui.hpp"
#include "../window.hpp"

class MasterHandler {
public:
    SceneHandler sceneHandler;
    tmine::Gui gui{};

    explicit MasterHandler(tmine::Window* window);
    
    void render(glm::uvec2 window_size);
    void updateAll(tmine::Window* window);
};
