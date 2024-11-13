#include "../game.hpp"
#include "../events.hpp"

namespace tmine {

Game::Game(glm::uvec2 viewport_size)
: scene{viewport_size}
, gui{}
, player{(f32) glfwGetTime(), -30.0f, vec3(0.0f)} {
    glClearColor(27.0 / 255.0, 26.0 / 255.0, 33.0 / 255.0, 1.0f);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(2.0f);
}

auto Game::render(this Game& self, glm::uvec2 viewport_size) -> void {
    if (!Window::is_visible(viewport_size)) {
        if (self.gui.current() == GuiState::InGame) {
            self.gui.set_state(GuiState::PauseMenu);
        }

        return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (self.gui.current() == GuiState::PauseMenu ||
        self.gui.current() == GuiState::InGame)
    {
        self.scene.render(self.player.cam, viewport_size);
    }

    if (self.gui.current() == GuiState::StartMenu ||
        self.gui.current() == GuiState::PauseMenu)
    {
        self.gui.render(viewport_size);
    }
}

auto Game::update(this Game& self, RefMut<Window> window) -> void {
    if (io.just_pressed(Key::T)) {
        window->toggle_cursor_visibility();
    }

    if (io.just_pressed(Key::Escape)) {
        self.gui.set_state(GuiState::PauseMenu);
        window->release_cursor();
    }

    if (GuiState::InGame == self.gui.current()) {
        auto& terrain = self.scene.get<Terrain>();
        auto& line_box = self.scene.get<LineBox>();

        self.player.update(&terrain, &line_box, window->get_size());
    }

    self.gui.update(window);
}

}  // namespace tmine
