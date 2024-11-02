#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <print>

#include "GUI/Text.h"
#include "graphics/MasterHandler.h"
#include "Window.h"

#include "events.hpp"
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

    Input::set_io_callbacks(Window::glfw_window);
    Text::init();
    MasterHandler::init();

    glClearColor(27.0 / 255.0, 26.0 / 255.0, 33.0 / 255.0, 1.0f);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(2.0f);

    while (!Window::isClosed()) {
        if (io.just_pressed(Key::T)) {
            io.toggle_cursor_visibility();
        }

        if (io.just_pressed(Key::Escape)) {
            MasterHandler::gui->current = pauseMenu;
            io.toggle_cursor_visibility();
        }

        MasterHandler::updateAll();
        MasterHandler::render();

        io.update();
        Window::swapBuffers();
        Window::pollEvents();
    }

    MasterHandler::terminate();
    Window::terminate();
}
