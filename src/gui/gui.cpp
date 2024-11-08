#include "../gui.hpp"
#include "../loaders.hpp"

namespace tmine {

Gui::Gui()
: font{load_font("assets/fonts/font.fnt")}
, button_style{ButtonStyle{
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
, shader{load_shader("gui_vertex.glsl", "gui_fragment.glsl")} {}

auto Gui::add_button(this Gui& self, std::string text, glm::vec2 pos, f32 size)
    -> void {
    auto key = text;

    self.objects.insert(
        {key, std::make_unique<Button>(
                  self.button_style,
                  Text{
                      self.font, self.button_style.glyph_texture,
                      std::move(text), pos, size
                  },
                  pos, size
              )}
    );
}

auto Gui::add_sprite(this Gui& self, std::string name, Sprite sprite) -> void {
    self.objects.insert(
        {std::move(name), std::make_unique<Sprite>(std::move(sprite))}
    );
}

}  // namespace tmine
