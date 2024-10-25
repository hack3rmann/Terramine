#include "GUIHandler.h"

#include "../Graphics/Texture.h"
#include "../Window.h"
#include "../EventHandler.h"

GUIHandler::GUIHandler(GUIstate current) {
    /* Init */
    GUIs = new GUI*[8];
    shader = load_shader("GUIVertex.glsl", "GUIFragment.glsl");
    this->current = startMenu;
    for (int i = 0; i < 8; i++) {
        GUIs[i] = nullptr;
    }

    /* Loading basic textures */
    bg = load_texture("assets/startScreenBackground.png");
    bDef = load_texture("assets/testButtonDef.png");
    bHover = load_texture("assets/testButtonHover.png");
    bClicked = load_texture("assets/testButtonClicked.png");
    darker = load_texture("assets/darker.png");

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
    shader->use();
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
    delete shader;

    bg->deleteTex();
    bDef->deleteTex();
    bHover->deleteTex();
    bClicked->deleteTex();

    delete bg;
    delete bDef;
    delete bHover;
    delete bClicked;
}
