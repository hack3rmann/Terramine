#pragma once

#include "gui.hpp"
#include "window.hpp"
#include "objects.hpp"
#include "Player.h"

namespace tmine {

class Game {
public:
    explicit Game(glm::uvec2 viewport_size);

    auto render(this Game& self, glm::uvec2 viewport_size) -> void;
    auto update(this Game& self, RefMut<Window> window) -> void;

private:
    Scene scene;
    Gui gui;
    Player player;
};

}  // namespace tmine
