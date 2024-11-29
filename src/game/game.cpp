#include "../game.hpp"
#include "../events.hpp"
#include "../debug.hpp"

namespace tmine {

namespace chrono = std::chrono;
using namespace std::literals;

static auto setup_opengl() -> void {
    glClearColor(27.0 / 255.0, 26.0 / 255.0, 33.0 / 255.0, 1.0f);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Game::Game(glm::uvec2 viewport_size)
: physics_solver{}
, scene{viewport_size}
, gui{GuiState::InGame}
, player{&this->physics_solver}
, debug{viewport_size}
, prev_time{chrono::high_resolution_clock::now()} {
    setup_opengl();

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
}

static auto draw_debug_text(glm::uvec2 viewport_size) -> void {
    debug::text()->set(
        "viewport",
        fmt::format("Viewport Size: {}x{}", viewport_size.x, viewport_size.y)
    );

    debug::text()->set(
        "gl",
        fmt::format(
            "Vendor: {}, Renderer: {}", (char const*) glGetString(GL_VENDOR),
            (char const*) glGetString(GL_RENDERER)
        )
    );

    debug::text()->set(
        "gl#version",
        fmt::format("OpenGL v{}", (char const*) glGetString(GL_VERSION))
    );
}

auto Game::render(this Game& self, glm::uvec2 viewport_size) -> void {
    if (!Window::is_visible(viewport_size)) {
        if (self.gui.current() == GuiState::InGame) {
            self.gui.set_state(GuiState::PauseMenu);
        }

        return;
    }

    draw_debug_text(viewport_size);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (self.gui.current() == GuiState::PauseMenu ||
        self.gui.current() == GuiState::InGame)
    {
        self.scene.render(self.player.get_camera(), viewport_size);

        debug::lines()->render(self.player.get_camera(), viewport_size);
        debug::text()->render(viewport_size);
    }

    if (self.gui.current() == GuiState::StartMenu ||
        self.gui.current() == GuiState::PauseMenu)
    {
        debug::lines()->render(self.player.get_camera(), viewport_size);
        self.gui.render(viewport_size);
    }
}

auto Game::update(this Game& self, RefMut<Window> window) -> void {
    auto const now = chrono::high_resolution_clock::now();
    auto const duration = chrono::duration<f32>{now - self.prev_time};
    self.prev_time = now;

    debug::text()->set(
        "fps", fmt::format("FPS: {:.1f}", 1.0f / duration.count())
    );

    debug::update();

    if (io.just_pressed(Key::T)) {
        window->toggle_cursor_visibility();
    }

    if (io.just_pressed(Key::Escape)) {
        self.gui.set_state(GuiState::PauseMenu);
        window->release_cursor();
    }

    if (GuiState::InGame == self.gui.current()) {
        self.physics_solver.update(duration.count());

        auto& terrain = self.scene.get<Terrain>();
        auto& selection = self.scene.get<SelectionBox>();

        self.player.update(
            &self.physics_solver, &terrain, &selection, window->size()
        );
    }

    self.gui.update(window);
}

}  // namespace tmine
