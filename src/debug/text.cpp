#include "../debug.hpp"
#include "../loaders.hpp"

namespace tmine {

auto constexpr DEBUG_TEXT_SIZE = 0.3f;

DebugText::DebugText(std::shared_ptr<Font> font)
: shader{load_shader("gui_vertex.glsl", "gui_fragment.glsl")}
, glyph_texture{Texture::from_image(
      load_png("assets/images/font.png"), TextureLoad::NO_MIPMAP_LINEAR
  )}
, font{font}
, viewport_size{1} {}

auto DebugText::render(this DebugText& self, glm::uvec2 viewport_size) -> void {
    self.viewport_size = viewport_size;

    if (!DEBUG_IS_ENABLED) {
        return;
    }

    for (auto& entry : self.text_lines) {
        auto& text = entry.second;
        text.render(self.shader, viewport_size);
    }
}

auto DebugText::set(
    this DebugText& self, StaticString element, std::string_view value
) -> void {
    auto const aspect_ratio = Window::aspect_ratio_of(self.viewport_size);
    auto const pos = glm::vec2{-0.8f * aspect_ratio, 0.8f};

    self.text_lines.insert_or_assign(
        std::move(element),
        Text{self.font, self.glyph_texture, value, pos, DEBUG_TEXT_SIZE}
    );
}

}  // namespace tmine
