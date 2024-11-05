#include "GUIHandler.h"

#include "../graphics.hpp"
#include "../window.hpp"
#include "../loaders.hpp"

using namespace tmine;

GUIHandler::GUIHandler([[maybe_unused]] GUIstate current, Window* window)
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
    guis[startMenu].addSprite(0.0f, 0.0f, 2.7f, 2.0f, bg);
    guis[startMenu].addButton(
        0.0f, 0.0f, 1.0f, 0.3f, bDef, bHover, bClicked, "Start",
        [&]() { this->current = nothing; }
    );
    guis[startMenu].addButton(
        0.0f, -0.4f, 1.0f, 0.3f, bDef, bHover, bClicked, "Exit",
        window->get_window_close_function()
    );

    auto toggle = window->get_window_toggle_cursor_function();

    /* Pause manu init */
    guis[pauseMenu].addSprite(0.0f, 0.0f, 2.0f, 2.0f, darker);
    guis[pauseMenu].addButton(
        0.0f, 0.2f, 1.0f, 0.3f, bDef, bHover, bClicked, "Return",
        [this, toggle]() {
            this->current = nothing;
            toggle();
        }
    );
    guis[pauseMenu].addButton(
        0.0f, -0.2f, 1.0f, 0.3f, bDef, bHover, bClicked, "Exit",
        [&]() { this->current = startMenu; }
    );
}

void GUIHandler::render(glm::uvec2 window_size) {
    shader.bind();
    guis[current].render(window_size);
}
