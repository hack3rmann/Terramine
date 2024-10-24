#include "MasterHandler.h"

#include "../defines.cpp"

/* Static fields init */
SceneHandler* MasterHandler::sceneHandler;
GUIHandler* MasterHandler::gui;

void MasterHandler::init() {
    sceneHandler = new SceneHandler();
    gui = new GUIHandler(startMenu);
}

void MasterHandler::render() {
    /* Normal rendering (if window not hidden) */
    if (Window::width + Window::height) {
        glcall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        if (gui->current == startMenu) {
            gui->render();
        } else if (gui->current == pauseMenu) {
            sceneHandler->render();
            gui->render();
        } else {
            sceneHandler->render();
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

void MasterHandler::updateAll() {
    /* Update will only if window not hidden */
    if (Window::width + Window::height) {
        if (!gui->current) {
            sceneHandler->updateAll();
        }
    }
}

void MasterHandler::terminate() {
    delete sceneHandler;
    delete gui;
}
