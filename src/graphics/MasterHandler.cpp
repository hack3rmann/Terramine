#include "MasterHandler.h"

#include "../window.hpp"

using namespace tmine;

MasterHandler::MasterHandler(Window* window)
: scene{window->get_size()}
, player{(f32) glfwGetTime(), -30.0f, vec3(0.0f)} {}

void MasterHandler::render(glm::uvec2 viewport_size) {
    if (!Window::is_visible(viewport_size)) {
        if (gui.current() == GuiState::InGame) {
            gui.set_state(GuiState::PauseMenu);
        }

        return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (gui.current() == GuiState::PauseMenu ||
        gui.current() == GuiState::InGame)
    {
        this->scene.render(this->player.cam, viewport_size);
    }

    if (gui.current() == GuiState::StartMenu ||
        gui.current() == GuiState::PauseMenu)
    {
        this->gui.render(viewport_size);
    }
}

void MasterHandler::updateAll(Window* window) {
    if (GuiState::InGame == this->gui.current()) {
        auto& terrain = this->scene.get<Terrain>();
        auto& line_box = this->scene.get<LineBox>();

        this->player.update(&terrain, &line_box, window->get_size());
    }

    this->gui.update(window);
}
