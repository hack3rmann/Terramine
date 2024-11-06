#include "MasterHandler.h"

#include "../window.hpp"

using namespace tmine;

MasterHandler::MasterHandler(Window* window)
: sceneHandler(window->get_size())
, gui(StartMenu, window) {}

void MasterHandler::render(glm::uvec2 window_size) {
    if (0 != window_size.x + window_size.y) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (gui.current == StartMenu) {
            gui.render(window_size);
        } else if (gui.current == PauseMenu) {
            sceneHandler.render(window_size);
            gui.render(window_size);
        } else {
            sceneHandler.render(window_size);
        }
    } else {
        if (gui.current == PauseMenu || gui.current == Nothing) {
            gui.current = PauseMenu;
        } else {
            gui.current = StartMenu;
        }
    }
}

void MasterHandler::updateAll(Window* window) {
    auto const window_size = window->get_size();

    if (0 != window_size.x + window_size.y && !gui.current) {
        sceneHandler.updateAll(window_size);
    }

    this->gui.update(window);
}
