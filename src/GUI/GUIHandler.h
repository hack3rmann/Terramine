#pragma once

#include "GUI.h"
#include "../graphics.hpp"
#include "../window.hpp"

enum GUIstate { nothing, startMenu, pauseMenu };

class GUIHandler {
    std::vector<GUI> guis;
    tmine::ShaderProgram shader;

    tmine::Texture bg;
    tmine::Texture bDef;
    tmine::Texture bHover;
    tmine::Texture bClicked;
    tmine::Texture darker;

public:
    GUIstate current;
    GUIHandler(GUIstate current, tmine::Window* window);
    void render(glm::uvec2 window_size);
};
