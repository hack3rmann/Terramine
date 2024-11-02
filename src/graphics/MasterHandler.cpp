#include "MasterHandler.h"

#include "../window.hpp"

using namespace tmine;

/* Static fields init */
SceneHandler* MasterHandler::sceneHandler;
GUIHandler* MasterHandler::gui;

void MasterHandler::init(Window* window) {
    sceneHandler = new SceneHandler(window->get_size());
    gui = new GUIHandler(startMenu, window);
}

void MasterHandler::render(glm::uvec2 window_size) {
    /* Normal rendering (if window not hidden) */
    if (0 != window_size.x + window_size.y) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (gui->current == startMenu) {
            gui->render(window_size);
        } else if (gui->current == pauseMenu) {
            sceneHandler->render(window_size);
            gui->render(window_size);
        } else {
            sceneHandler->render(window_size);
        }
    }
    /* Hidden window */
    else
    {
        if (gui->current == pauseMenu || gui->current == nothing) {
            gui->current = pauseMenu;
        } else {
            gui->current = startMenu;
        }
    }
}

void MasterHandler::updateAll(glm::uvec2 window_size) {
    /* Update will only if window not hidden */
    if (0 != window_size.x + window_size.y && !gui->current) {
        sceneHandler->updateAll(window_size);
    }
}

void MasterHandler::terminate() {
    delete sceneHandler;
    delete gui;
}
