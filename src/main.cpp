#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "EventHandler.h"
#include "GUI/Text.h"
#include "graphics/MasterHandler.h"
#include "Window.h"

#include "types.hpp"

using namespace tmine;

static usize constexpr INITIAL_WINDOW_WIDTH = 640;
static usize constexpr INITIAL_WINDOW_HEIGHT = 480;

auto main() -> int {
    if (Window::init(
            INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, "Terramine"
        ) != 0)
    {
        std::fprintf(stderr, "failed to initialize a window\n");
        return -1;
    }

    Events::init();
    Text::init();
    MasterHandler::init();

    glClearColor(0.61f, 0.86f, 1.0f, 1.0f);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(2.0f);

    while (!Window::isClosed()) {
        if (Events::justPressed(GLFW_KEY_T)) {
            Events::toggleCursor();
        }

        if (Events::justPressed(GLFW_KEY_ESCAPE)) {
            MasterHandler::gui->current = pauseMenu;
            Events::toggleCursor();
        }

        MasterHandler::updateAll();
        MasterHandler::render();

        Window::swapBuffers();
        Events::pullEvents();
    }

    MasterHandler::terminate();
    Events::terminate();
    Window::terminate();

    return 0;
}
