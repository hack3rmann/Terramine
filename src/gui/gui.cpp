#include "../gui.hpp"
#include "../loaders.hpp"
#include "../panic.hpp"

namespace tmine {

Gui::Gui()
: button_style{ButtonStyle{
      .textures =
          {
              Texture::from_image(
                  load_png("assets/images/testButtonDef.png"),
                  TextureLoad::DEFAULT
              ),
              Texture::from_image(
                  load_png("assets/images/testButtonHover.png"),
                  TextureLoad::DEFAULT
              ),
              Texture::from_image(
                  load_png("assets/images/testButtonClicked.png"),
                  TextureLoad::DEFAULT
              ),
          },
      .glyph_texture = Texture::from_image(
          load_png("assets/images/font.png"), TextureLoad::NO_MIPMAP_LINEAR
      ),
  }}
, font{load_font("assets/fonts/font.fnt")}
, shader{load_shader("gui_vertex.glsl", "gui_fragment.glsl")} {}

auto Gui::add_sprite(this Gui& self, Sprite sprite) -> void {
    self.sprites.emplace_back(std::move(sprite));
}

auto Gui::add_button(
    this Gui& self, std::string_view text, glm::vec2 pos, f32 size
) -> void {
    self.buttons.insert(
        {text,
         Button{
             self.button_style,
             Text{
                 self.font, self.button_style.glyph_texture, std::string{text},
                 pos, size
             },
             pos, size
         }}
    );
}

auto Gui::get_button(this Gui const& self, std::string_view name)
    -> Button const& {
    if (!self.buttons.contains(name)) {
        throw Panic("there is no button with name '{}'", name);
    }

    return self.buttons.at(name);
}

auto Gui::render(this Gui& self, glm::uvec2 viewport_size) -> void {
    for (auto& sprite : self.sprites) {
        sprite.render(self.shader, viewport_size);
    }

    for (auto& elem : self.buttons) {
        // C++ just can't use reference structural binding without copying
        // so we can manually unpack button reference
        auto& button = elem.second;

        button.render(self.shader, viewport_size);
    }
}

}  // namespace tmine
