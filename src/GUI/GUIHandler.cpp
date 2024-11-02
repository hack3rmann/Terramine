#include "GUIHandler.h"

#include "../graphics.hpp"
#include "../window.hpp"
#include "../loaders.hpp"

using namespace tmine;

GUIHandler::GUIHandler([[maybe_unused]] GUIstate current, Window* window) {
    /* Init */
    GUIs = new GUI*[8];
    shader = ShaderProgram::from_source(
        load_shader_source("GUIVertex.glsl", "GUIFragment.glsl")
    );
    this->current = startMenu;
    for (int i = 0; i < 8; i++) {
        GUIs[i] = nullptr;
    }

    /* Loading basic textures */
    bg = Texture::from_image(
        load_png("assets/startScreenBackground.png"), TextureLoad::DEFAULT
    );
    bDef = Texture::from_image(
        load_png("assets/testButtonDef.png"), TextureLoad::DEFAULT
    );
    bHover = Texture::from_image(
        load_png("assets/testButtonHover.png"), TextureLoad::DEFAULT
    );
    bClicked = Texture::from_image(
        load_png("assets/testButtonClicked.png"), TextureLoad::DEFAULT
    );
    darker = Texture::from_image(
        load_png("assets/darker.png"), TextureLoad::DEFAULT
    );

    /* Empty menu init */
    GUIs[nothing] = new GUI();

    /* Start Menu init */
    GUIs[startMenu] = new GUI();
    GUIs[startMenu]->addSprite(0.0f, 0.0f, 2.0f, 2.0f, bg);
    GUIs[startMenu]->addButton(
        0.0f, 0.0f, 1.0f, 0.3f, bDef, bHover, bClicked, "Start",
        [&]() { this->current = nothing; }
    );
    GUIs[startMenu]->addButton(
        0.0f, -0.4f, 1.0f, 0.3f, bDef, bHover, bClicked, "Exit",
        window->get_window_close_function()
    );

    auto toggle = window->get_window_toggle_cursor_function();

    /* Pause manu init */
    GUIs[pauseMenu] = new GUI();
    GUIs[pauseMenu]->addSprite(0.0f, 0.0f, 2.0f, 2.0f, darker);
    GUIs[pauseMenu]->addButton(
        0.0f, 0.2f, 1.0f, 0.3f, bDef, bHover, bClicked, "Return",
        [this, toggle]() {
            this->current = nothing;
            toggle();
        }
    );
    GUIs[pauseMenu]->addButton(
        0.0f, -0.2f, 1.0f, 0.3f, bDef, bHover, bClicked, "Exit",
        [&]() { this->current = startMenu; }
    );
}

void GUIHandler::render(glm::uvec2 window_size) {
    shader.bind();
    GUIs[current]->render(window_size);
}

GUIHandler::~GUIHandler() {
    for (int i = 0; i < 8; i++) {
        if (GUIs[i] == nullptr) {
            continue;
        }
        GUIs[i]->cleanUp();
        delete GUIs[i];
    }
    delete[] GUIs;
}
