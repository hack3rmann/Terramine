#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/ext.hpp>
#include <fmt/printf.h>

#include "graphics/MasterHandler.h"
#include "window.hpp"

#include "events.hpp"

using namespace tmine;

auto main() -> int {
    auto window = Window{"Terramine"};

    Input::set_io_callbacks(window.get_glfw_window());
    Text::init();
    MasterHandler::init(&window);

    glClearColor(27.0 / 255.0, 26.0 / 255.0, 33.0 / 255.0, 1.0f);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(2.0f);

    while (window.is_open()) {
        if (io.just_pressed(Key::T)) {
            window.toggle_cursor_visibility();
        }

        if (io.just_pressed(Key::Escape)) {
            MasterHandler::gui->current = pauseMenu;
            window.toggle_cursor_visibility();
        }

        MasterHandler::updateAll(window.get_size());
        MasterHandler::render(window.get_size());

        io.update();
        window.swap_buffers();
        window.poll_events();
    }

    MasterHandler::terminate();
}
