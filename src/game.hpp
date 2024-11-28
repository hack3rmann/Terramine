#pragma once

#include <chrono>

#include "gui.hpp"
#include "window.hpp"
#include "objects.hpp"
#include "physics.hpp"
#include "debug.hpp"

namespace tmine {

class Game {
public:
    explicit Game(glm::uvec2 viewport_size);

    auto render(this Game& self, glm::uvec2 viewport_size) -> void;
    auto update(this Game& self, RefMut<Window> window) -> void;

private:
    PhysicsSolver physics_solver;
    Scene scene;
    Gui gui;
    Player player;
    DebugOwner debug;
    std::chrono::time_point<std::chrono::high_resolution_clock> prev_time;
};

}  // namespace tmine
