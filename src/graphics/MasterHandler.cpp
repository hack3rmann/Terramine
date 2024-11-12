#include "MasterHandler.h"

#include "../window.hpp"

using namespace tmine;

MasterHandler::MasterHandler(Window* window)
: scene{window->get_size()}
, player{(f32) glfwGetTime(), -30.0f, vec3(0.0f)} {}

void MasterHandler::render(glm::uvec2 viewport_size) {
    if (0 != viewport_size.x + viewport_size.y) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (gui.current() == GuiState::StartMenu) {
            gui.render(viewport_size);
        } else if (gui.current() == GuiState::PauseMenu) {
            this->scene.render(this->player.cam, viewport_size);
            gui.render(viewport_size);
        } else {
            this->scene.render(this->player.cam, viewport_size);
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
    if (GuiState::None == this->gui.current()) {
        this->scene.update_player(&this->player);
    }

    this->gui.update(window);
}
