#pragma once

#include "../graphics/Shader.h"
#include "GUI.h"

enum GUIstate { nothing, startMenu, pauseMenu, settings };

class GUIHandler {
    GUI** GUIs;
    Shader* shader;

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
