#define GLM_FORCE_SWIZZLE
#include <glm/ext.hpp>

#include "../window.hpp"
#include "../events.hpp"
#include "../gui.hpp"

namespace tmine {

Button::Button(ButtonStyle style, Text text, glm::vec2 pos, f32 size)
: style{std::move(style)}
, text{std::move(text)}
, pos{pos}
, size{size} {
    auto const texture_size = this->style.textures[0].get_size();
    auto const aspect_ratio = (f32) texture_size.y / (f32) texture_size.x;

    auto& buffer = this->mesh.get_buffer();

    Button::add_gui_rect(&buffer, pos, glm::vec2{size, aspect_ratio * size});

    this->mesh.reload_buffer();
}

auto Button::get_size(this Button const& self) -> glm::vec2 {
    auto const texture_size = self.style.textures[0].get_size();
    auto const aspect_ratio = (f32) texture_size.y / (f32) texture_size.x;

    return glm::vec2{self.size, aspect_ratio * self.size};
}

auto Button::render(ShaderProgram const& shader, glm::uvec2 viewport_size)
    -> void {
    this->style.textures[(usize) this->state].bind(0);

    auto const aspect_ratio = Window::aspect_ratio_of(viewport_size);
    auto const proj =
        glm::ortho(-aspect_ratio, aspect_ratio, -1.0f, 1.0f, 0.0f, 100.0f);

    shader.bind();
    shader.uniform_mat4("model_projection", proj);

    mesh.draw();

    this->text.render(shader, viewport_size);
}

auto Button::update_state(this Button& self, glm::uvec2 viewport_size) -> void {
    auto const aspect_ratio = Window::aspect_ratio_of(viewport_size);
    auto const transform =
        glm::ortho(-aspect_ratio, aspect_ratio, -1.0f, 1.0f, 0.0f, 100.0f);

    auto mouse_pos_global =
        2.0f * io.get_mouse_pos() / glm::vec2{viewport_size} - 1.0f;
    auto mouse_pos_local =
        glm::inverse(transform) *
        glm::vec4{mouse_pos_global.x, -mouse_pos_global.y, 0.0f, 1.0f};

    auto size = self.get_size();
    auto abs_diff = glm::abs(mouse_pos_local.xy() - self.pos);
    auto button_is_hovered =
        abs_diff.x <= 0.5f * size.x && abs_diff.y <= 0.5f * size.y;

    if (button_is_hovered) {
        if (io.is_clicked(MouseButton::Left)) {
            self.state = ButtonState::Clicked;
        } else {
            self.state = ButtonState::Hovered;
        }
    } else {
        self.state = ButtonState::Default;
    }
}

}  // namespace tmine
