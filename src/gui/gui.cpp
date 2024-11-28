#include "../gui.hpp"
#include "../loaders.hpp"
#include "../panic.hpp"

namespace tmine {

GuiStage::GuiStage(std::shared_ptr<Font> font)
: button_style{ButtonStyle{
      .textures =
          {
              Texture::from_image(
                  load_png("assets/images/button_default.png"),
                  TextureLoad::DEFAULT
              ),
              Texture::from_image(
                  load_png("assets/images/button_hovered.png"),
                  TextureLoad::DEFAULT
              ),
              Texture::from_image(
                  load_png("assets/images/button_clicked.png"),
                  TextureLoad::DEFAULT
              ),
          },
      .glyph_texture = Texture::from_image(
          load_png("assets/images/font.png"), TextureLoad::NO_MIPMAP_LINEAR
      ),
  }}
, font{std::move(font)}
, shader{load_shader("gui_vertex.glsl", "gui_fragment.glsl")} {}

auto GuiStage::add_sprite(this GuiStage& self, Sprite sprite) -> void {
    self.sprites.emplace_back(std::move(sprite));
}

auto GuiStage::add_button(
    this GuiStage& self, StaticString text, glm::vec2 pos, f32 size
) -> void {
    self.buttons.insert(
        {text,
         Button{
             self.button_style,
             Text{self.font, self.button_style.glyph_texture, text, pos, size},
             pos, size
         }}
    );
}

auto GuiStage::get_button(this GuiStage const& self, std::string_view name)
    -> Button const& {
    if (!self.buttons.contains(name)) {
        throw Panic("there is no button with name '{}'", name);
    }

    return self.buttons.at(name);
}

auto GuiStage::render(this GuiStage& self, glm::uvec2 viewport_size) -> void {
    for (auto& sprite : self.sprites) {
        sprite.render(self.shader, viewport_size);
    }

    for (auto& elem : self.buttons) {
        // C++ just unable to use structural binding by reference without
        // copying so we should manually unpack button reference
        auto& button = elem.second;

        button.render(self.shader, viewport_size);
    }
}

auto GuiStage::update(this GuiStage& self, glm::uvec2 viewport_size) -> void {
    for (auto& elem : self.buttons) {
        auto& button = elem.second;

        button.update_state(viewport_size);
    }
}

auto Gui::add_stage(this Gui& self, GuiState state) -> GuiStage& {
    auto [iter, _] = self.guis.insert({state, GuiStage{self.font}});
    return iter->second;
}

Gui::Gui(GuiState initial_state)
: state{initial_state}
, font{std::make_shared<Font>(load_font("assets/fonts/font.fnt"))} {
    auto background_texture =
        Texture::from_image(load_png("assets/images/startScreenBackground.png")
        );
    auto black_texture =
        Texture::from_image(load_png("assets/images/darker.png"));

    this->add_stage(GuiState::InGame);

    auto& start_menu = this->add_stage(GuiState::StartMenu);
    start_menu.add_sprite({glm::vec2{0.0f, 0.0f}, 2.7f, background_texture});
    start_menu.add_button("Start", glm::vec2{0.0f, 0.0f}, 1.0f);
    start_menu.add_button("Exit", glm::vec2{0.0f, -0.4f}, 1.0f);

    auto& pause_menu = this->add_stage(GuiState::PauseMenu);
    pause_menu.add_sprite({glm::vec2{0.0f, 0.0f}, 100.0f, black_texture});
    pause_menu.add_button("Return", glm::vec2{0.0f, 0.2f}, 1.0f);
    pause_menu.add_button("Exit", glm::vec2{0.0f, -0.2f}, 1.0f);
}

auto Gui::render(this Gui& self, glm::uvec2 viewport_size) -> void {
    self.guis.at(self.state).render(viewport_size);
}

auto Gui::update(this Gui& self, Window* window) -> void {
    for (auto& entry : self.guis) {
        auto& gui = entry.second;
        gui.update(window->size());
    }

    auto& stage = self.guis.at(self.state);

    switch (self.state) {
    case GuiState::InGame: {
    } break;
    case GuiState::StartMenu: {
        if (stage.get_button("Start").clicked()) {
            window->capture_cursor();
            self.state = GuiState::InGame;
        }

        if (stage.get_button("Exit").clicked()) {
            window->release_cursor();
            window->schedule_close();
        }
    } break;
    case GuiState::PauseMenu: {
        if (stage.get_button("Return").clicked()) {
            self.set_state(GuiState::InGame);
            window->capture_cursor();
        }

        if (stage.get_button("Exit").clicked()) {
            self.set_state(GuiState::StartMenu);
            window->release_cursor();
        }
    } break;
    }
}

}  // namespace tmine
