#include "GUIHandler.h"

#include "../graphics.hpp"
#include "../Window.h"
#include "../EventHandler.h"

#include "../loaders.hpp"

using namespace tmine;

GUIHandler::GUIHandler([[maybe_unused]] GUIstate current) {
    /* Init */
    GUIs = new GUI*[8];
    shader = ShaderProgram::from_source(load_shader("GUIVertex.glsl", "GUIFragment.glsl").value()).value();
    this->current = startMenu;
    for (int i = 0; i < 8; i++) {
        GUIs[i] = nullptr;
    }

    /* Loading basic textures */
    bg = Texture::from_image(
        load_png("assets/startScreenBackground.png").value(),
        TextureLoad::DEFAULT
    );
    bDef = Texture::from_image(
        load_png("assets/testButtonDef.png").value(), TextureLoad::DEFAULT
    );
    bHover = Texture::from_image(
        load_png("assets/testButtonHover.png").value(), TextureLoad::DEFAULT
    );
    bClicked = Texture::from_image(
        load_png("assets/testButtonClicked.png").value(), TextureLoad::DEFAULT
    );
    darker = Texture::from_image(
        load_png("assets/darker.png").value(), TextureLoad::DEFAULT
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
        [&]() { Window::setShouldClose(true); }
    );

    /* Pause manu init */
    GUIs[pauseMenu] = new GUI();
    GUIs[pauseMenu]->addSprite(0.0f, 0.0f, 2.0f, 2.0f, darker);
    GUIs[pauseMenu]->addButton(
        0.0f, 0.2f, 1.0f, 0.3f, bDef, bHover, bClicked, "Return",
        [&]() {
            this->current = nothing;
            Events::toggleCursor();
        }
    );
    GUIs[pauseMenu]->addButton(
        0.0f, -0.2f, 1.0f, 0.3f, bDef, bHover, bClicked, "Exit",
        [&]() { this->current = startMenu; }
    );
}

void GUIHandler::render() {
    shader.bind();
    GUIs[current]->render();
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
