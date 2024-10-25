#pragma once

#include "GUI.h"
#include "../graphics.hpp"

enum GUIstate { nothing, startMenu, pauseMenu, settings };

class GUIHandler {
    GUI** GUIs;
    tmine::ShaderProgram shader;

    tmine::Texture bg;
    tmine::Texture bDef;
    tmine::Texture bHover;
    tmine::Texture bClicked;
    tmine::Texture darker;

public:
    GUIstate current;
    GUIHandler(GUIstate current);
    ~GUIHandler();
    void render();
};
