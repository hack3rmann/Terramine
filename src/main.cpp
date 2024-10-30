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
#include "loaders.hpp"

using namespace tmine;

static usize constexpr INITIAL_WINDOW_WIDTH = 640;
static usize constexpr INITIAL_WINDOW_HEIGHT = 480;

auto main() -> int {
    auto const textures =
        load_game_block_textures("assets/block_textures.json").value();
    auto const blocks =
        load_game_blocks("assets/blocks.json", textures).value();

    for (auto const& block : blocks) {
        std::println(
            "NAME={}, ID={}, META={}, [{}, {}, {}, {}, {}, {}]", block.name,
            block.voxel_id, block.meta, block.texture_ids[0],
            block.texture_ids[1], block.texture_ids[2], block.texture_ids[3],
            block.texture_ids[4], block.texture_ids[5]
        );
    }

    return 0;

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

    glClearColor(0.61f, 0.86f, 1.0f, 1.0f);
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
