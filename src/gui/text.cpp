#include "../window.hpp"
#include "../panic.hpp"
#include "../gui.hpp"

#include <glm/ext.hpp>

namespace tmine {

namespace rg = std::ranges;

static auto add_quad(
    std::vector<GuiObject::Vertex>* buffer, glm::vec2 pos, glm::vec2 size,
    glm::vec2 uv, glm::vec2 uv_size
) -> void {
    auto const vertices = std::array{
        GuiObject::Vertex{
            glm::vec2{pos.x, pos.y + size.y}, glm::vec2{uv.x, uv.y}
        },
        GuiObject::Vertex{
            glm::vec2{pos.x, pos.y}, glm::vec2{uv.x, uv.y - uv_size.y}
        },
        GuiObject::Vertex{
            glm::vec2{pos.x + size.x, pos.y},
            glm::vec2{uv.x + uv_size.x, uv.y - uv_size.y}
        },
        GuiObject::Vertex{
            glm::vec2{pos.x, pos.y + size.y}, glm::vec2{uv.x, uv.y}
        },
        GuiObject::Vertex{
            glm::vec2{pos.x + size.x, pos.y + size.y},
            glm::vec2{uv.x + uv_size.x, uv.y}
        },
        GuiObject::Vertex{
            glm::vec2{pos.x + size.x, pos.y},
            glm::vec2{uv.x + uv_size.x, uv.y - uv_size.y}
        },
    };

    buffer->insert(buffer->end(), vertices.begin(), vertices.end());
}

/// # Safety
///
/// `font` should have at least one page
///
/// # Errors
///
/// Throws PanicException if symbol is not alphanumeric
static auto add_glyph(
    std::vector<GuiObject::Vertex>* buffer, Font const& font, f32 offset,
    f32 size, char symbol
) -> f32 {
    auto const& first_page = font.pages.front();
    auto const& desc = first_page.chars[(usize) symbol];
    auto const y = (i32) font.common.line_height / 2 - (i32) desc.offset.y -
                   (i32) desc.size.y;

    auto const pos = glm::vec2{offset, size * (f32) y / (f32) font.common.scale.y};

    add_quad(
        buffer, pos, size * glm::vec2{desc.size} / glm::vec2{font.common.scale},
        glm::vec2{desc.pos.x, (i32) font.common.scale.y - (i32) desc.pos.y} /
            glm::vec2{font.common.scale},
        glm::vec2{desc.size} / glm::vec2{font.common.scale}
    );

    return size * (f32) desc.horizontal_advance / (f32) font.common.scale.x;
}

Text::Text(
    std::shared_ptr<Font> font, Texture glyph_texture, std::string_view text,
    glm::vec2 pos, f32 size
)
: mesh{Primitive::Triangles}
, font{font}
, glyph_texture{std::move(glyph_texture)}
, pos{pos}
, size{size} {
    if (font->pages.empty()) {
        throw Panic("font {} contains no pages", font->info.face);
    }

    this->set_text(text);
}

auto Text::set_text(this Text& self, std::string_view text) -> void {
    auto const& first_page = self.font->pages.front();
    auto const scale_width = self.font->common.scale.x;

    auto const length =
        self.size / (f32) scale_width *
        (f32) rg::fold_left(text, 0, [&](u32 acc, char elem) {
            return acc + first_page.chars[(usize) elem].horizontal_advance;
        });

    auto& buffer = self.mesh.get_buffer();
    buffer.clear();

    auto offset = -0.5f * length;

    for (auto symbol : text) {
        offset += add_glyph(&buffer, *self.font, offset, self.size, symbol);
    }

    self.mesh.reload_buffer();
}

auto Text::render(ShaderProgram const& shader, glm::uvec2 viewport_size)
    -> void {
    shader.bind();
    this->glyph_texture.bind(0);

    glDisable(GL_DEPTH_TEST);

    auto const aspect_ratio = Window::aspect_ratio_of(viewport_size);
    auto const proj =
        glm::ortho(-aspect_ratio, aspect_ratio, -1.0f, 1.0f, 0.0f, 100.0f);

    auto const model =
        glm::translate(glm::mat4{1.0f}, glm::vec3(this->pos, 0.0f));

    shader.uniform_mat4("model_projection", proj * model);

    mesh.draw();
}

}  // namespace tmine
