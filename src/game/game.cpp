#include "../game.hpp"
#include "../events.hpp"
#include "../debug.hpp"

namespace tmine {

namespace chrono = std::chrono;
using namespace std::literals;

Game::Game(glm::uvec2 viewport_size)
: physics_solver{}
, scene{viewport_size}
, gui{GuiState::InGame}
, player{(f32) glfwGetTime(), -30.0f, vec3(0.0f)}
, debug{}
, prev_time{chrono::high_resolution_clock::now()} {
    glClearColor(27.0 / 255.0, 26.0 / 255.0, 33.0 / 255.0, 1.0f);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(5.0f);

    // It is okay if no terrain is found
    try {
        auto& terrain = this->scene.get<Terrain>();
        auto chunk_array = terrain.borrow_array();
        this->physics_solver.register_collidable<TerrainCollider>(chunk_array);
    } catch (PanicException const& panic) {
        fmt::print(
            stderr, "warning: no terrain in the scene: {}", panic.what()
        );
    }

    auto const lo = glm::vec3{60.0f};
    this->player_collidable_id =
        this->physics_solver.register_collidable<BoxCollider>(
            Aabb{lo, lo + glm::vec3{1.0f}}, glm::vec3{0.0f},
            ABSOLUTELY_INELASTIC_ELASTICITY
        );
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

    debug::lines()->render(self.player.cam, viewport_size);
}

auto Game::update(this Game& self, RefMut<Window> window) -> void {
    auto const now = chrono::high_resolution_clock::now();
    auto const duration = chrono::duration<f32>{now - self.prev_time};
    self.prev_time = now;

    if (duration.count() < 1.0f) {
        self.physics_solver.update(duration.count());
    }

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
        auto& player_collidable =
            self.physics_solver.get_collidable<BoxCollider>(
                self.player_collidable_id
            );

        self.player.update(
            &player_collidable, &terrain, &line_box, window->get_size()
        );
    }

    self.gui.update(window);
}

}  // namespace tmine
