#include "GUIHandler.h"

#include "../graphics.hpp"
#include "../window.hpp"
#include "../loaders.hpp"

using namespace tmine;

GUIHandler::GUIHandler(GUIstate current)
: guis{3}
, current{current} {
    /* Init */
    shader = ShaderProgram::from_source(
        load_shader_source("gui_vertex.glsl", "gui_fragment.glsl")
    );

    /* Loading basic textures */
    bg = Texture::from_image(
        load_png("assets/images/startScreenBackground.png"),
        TextureLoad::DEFAULT
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
    guis[StartMenu].add_sprite({glm::vec2{0.0f, 0.0f}, 2.7f, bg});
    guis[StartMenu].add_button("Start", glm::vec2{0.0f, 0.0f}, 1.0f);
    guis[StartMenu].add_button("Exit", glm::vec2{0.0f, -0.4f}, 1.0f);

    /* Pause manu init */
    guis[PauseMenu].add_sprite({glm::vec2{0.0f, 0.0f}, 2.0f, darker});
    guis[PauseMenu].add_button("Return", glm::vec2{0.0f, 0.2f}, 1.0f);
    guis[PauseMenu].add_button("Exit", glm::vec2{0.0f, -0.2f}, 1.0f);
}

void GUIHandler::render(glm::uvec2 window_size) {
    guis[current].render(window_size);
}

auto GUIHandler::update(this GUIHandler& self, Window* window) -> void {
    if (self.current == StartMenu &&
        self.guis[StartMenu].get_button("Start").clicked())
    {
        self.current = Nothing;
    } else if (self.current == StartMenu &&
               self.guis[StartMenu].get_button("Exit").clicked())
    {
        window->schedule_close();
    } else if (self.current == PauseMenu &&
               self.guis[PauseMenu].get_button("Return").clicked())
    {
        self.current = Nothing;
        window->toggle_cursor_visibility();
    } else if (self.current == PauseMenu &&
               self.guis[PauseMenu].get_button("Exit").clicked())
    {
        self.current = StartMenu;
    }
}
