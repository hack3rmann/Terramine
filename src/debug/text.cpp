#include "../debug.hpp"
#include "../loaders.hpp"

namespace tmine {

namespace vs = std::ranges::views;

auto constexpr DEBUG_TEXT_SIZE = 0.3f;
auto constexpr MONITOR_FONT_SCALE = 1623.0f;
auto constexpr EDGE_OFFSET = 0.025f;
auto constexpr LINE_MARGIN = 0.01f;

static auto font_size_of(glm::uvec2 viewport_size) -> f32 {
    return DEBUG_TEXT_SIZE * MONITOR_FONT_SCALE / (f32) viewport_size.y;
}

static auto line_height_of(Font const& font, glm::uvec2 viewport_size) -> f32 {
    return font_size_of(viewport_size) * (f32) font.common.line_height /
           (f32) font.common.scale.y;
}

DebugText::DebugText(glm::uvec2 viewport_size)
: shader{load_shader("gui_vertex.glsl", "gui_fragment.glsl")}
, glyph_texture{Texture::from_image(
      load_png("assets/images/debug_font.png"), TextureLoad::NO_MIPMAP_LINEAR
  )}
, font{std::make_shared<Font>(load_font("assets/fonts/debug_font.fnt"))}
, viewport_size{viewport_size} {}

auto DebugText::render(this DebugText& self, glm::uvec2 viewport_size) -> void {
    self.viewport_size = viewport_size;

    if (!DEBUG_IS_ENABLED) {
        return;
    }

    auto const line_height = line_height_of(*self.font, viewport_size);
    auto vertical_offset = 1.0f - EDGE_OFFSET - 0.5f * line_height;

    for (auto [i, entry] : self.text_lines | vs::enumerate) {
        auto& text = entry.second;

        auto position = text.get_position();
        position.y = vertical_offset;
        vertical_offset -= line_height + LINE_MARGIN;
        text.set_position(position);

        text.render(self.shader, viewport_size);
    }
}

auto DebugText::set(
    this DebugText& self, StaticString element, std::string_view value
) -> void {
    auto const font_size = font_size_of(self.viewport_size);
    auto const aspect_ratio = Window::aspect_ratio_of(self.viewport_size);

    auto pos = glm::vec2{-1.0f * aspect_ratio, 1.0f};
    auto text = Text{self.font, self.glyph_texture, value, pos, font_size};

    pos.x += 0.5f * text.get_width() + EDGE_OFFSET;
    text.set_position(pos);

    if (self.text_lines.contains(element)) {
        self.text_lines.erase(element);
    }

    self.text_lines.insert({std::move(element), std::move(text)});
}

}  // namespace tmine
