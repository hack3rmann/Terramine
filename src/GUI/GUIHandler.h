#pragma once

#include "../gui.hpp"
#include "../graphics.hpp"
#include "../window.hpp"

enum GUIstate { Nothing, StartMenu, PauseMenu };

class GUIHandler {
    std::vector<tmine::Gui> guis;
    tmine::ShaderProgram shader;

    tmine::Texture bg;
    tmine::Texture bDef;
    tmine::Texture bHover;
    tmine::Texture bClicked;
    tmine::Texture darker;

public:
    GUIstate current;
    GUIHandler(GUIstate current);
    void render(glm::uvec2 window_size);
    auto update(this GUIHandler& self, tmine::Window* window) -> void;
};
