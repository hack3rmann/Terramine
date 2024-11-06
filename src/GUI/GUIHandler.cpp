#include "GUIHandler.h"

#include "../graphics.hpp"
#include "../window.hpp"
#include "../loaders.hpp"

using namespace tmine;

GUIHandler::GUIHandler(GUIstate current, Window* window)
: guis{3}
, current{current} {
    /* Init */
    shader = ShaderProgram::from_source(
        load_shader_source("gui_vertex.glsl", "gui_fragment.glsl")
    );

    /* Loading basic textures */
    bg = Texture::from_image(
        load_png("assets/images/startScreenBackground.png"), TextureLoad::DEFAULT
    );
    bDef = Texture::from_image(
        load_png("assets/images/testButtonDef.png"), TextureLoad::DEFAULT
    );
    bHover = Texture::from_image(
        load_png("assets/images/testButtonHover.png"), TextureLoad::DEFAULT
    );
    bClicked = Texture::from_image(
        load_png("assets/images/testButtonClicked.png"), TextureLoad::DEFAULT
    );
    darker = Texture::from_image(
        load_png("assets/images/darker.png"), TextureLoad::DEFAULT
    );

    /* Start Menu init */
    guis[StartMenu].addSprite(0.0f, 0.0f, 2.7f, 2.0f, bg);
    guis[StartMenu].addButton(
        0.0f, 0.0f, 1.0f, 0.3f, bDef, bHover, bClicked, "Start",
        [&]() { this->current = Nothing; }
    );
    guis[StartMenu].addButton(
        0.0f, -0.4f, 1.0f, 0.3f, bDef, bHover, bClicked, "Exit",
        window->get_window_close_function()
    );

    auto toggle = window->get_window_toggle_cursor_function();

    /* Pause manu init */
    guis[PauseMenu].addSprite(0.0f, 0.0f, 2.0f, 2.0f, darker);
    guis[PauseMenu].addButton(
        0.0f, 0.2f, 1.0f, 0.3f, bDef, bHover, bClicked, "Return",
        [this, toggle]() {
            this->current = Nothing;
            toggle();
        }
    );
    guis[PauseMenu].addButton(
        0.0f, -0.2f, 1.0f, 0.3f, bDef, bHover, bClicked, "Exit",
        [&]() { this->current = StartMenu; }
    );
}

void GUIHandler::render(glm::uvec2 window_size) {
    guis[current].render(window_size);
}

auto GUIHandler::update(this GUIHandler& self, Window* window) -> void {
    if (self.current == StartMenu && self.guis[StartMenu].buttons[0].clicked()) {
        self.current = Nothing;
    } else if (self.current == StartMenu && self.guis[StartMenu].buttons[1].clicked()) {
        window->schedule_close();
    } else if (self.current == PauseMenu && self.guis[PauseMenu].buttons[0].clicked()) {
        self.current = Nothing;
        window->toggle_cursor_visibility();
    } else if (self.current == PauseMenu && self.guis[PauseMenu].buttons[1].clicked()) {
        self.current = StartMenu;
    }
}
