#include "MasterHandler.h"

#include "../window.hpp"

using namespace tmine;

MasterHandler::MasterHandler(Window* window)
: sceneHandler(window->get_size()) {}

void MasterHandler::render(glm::uvec2 viewport_size) {
    if (0 != viewport_size.x + viewport_size.y) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (gui.current() == GuiState::StartMenu) {
            gui.render(viewport_size);
        } else if (gui.current() == GuiState::PauseMenu) {
            sceneHandler.render(viewport_size);
            gui.render(viewport_size);
        } else {
            sceneHandler.render(viewport_size);
        }
    } else {
        if (gui.current() == GuiState::PauseMenu || gui.current() == GuiState::None) {
            gui.set_state(GuiState::PauseMenu);
        } else {
            gui.set_state(GuiState::StartMenu);
        }
    }
}

void MasterHandler::updateAll(Window* window) {
    auto const viewport_size = window->get_size();

    if (0 != viewport_size.x + viewport_size.y && gui.current() == GuiState::None) {
        sceneHandler.updateAll(viewport_size);
    }

    this->gui.update(window);
}
