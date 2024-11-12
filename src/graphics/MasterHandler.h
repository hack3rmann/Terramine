#pragma once

#include <glad/gl.h>

#include "../gui.hpp"
#include "../window.hpp"
#include "../objects.hpp"
#include "../Player.h"

class MasterHandler {
public:
    tmine::Scene scene;
    tmine::Gui gui{};
    Player player;

    explicit MasterHandler(tmine::Window* window);
    
    void render(glm::uvec2 window_size);
    void updateAll(tmine::Window* window);
};
