#include <glm/ext.hpp>

#include "../window.hpp"
#include "../gui.hpp"

namespace tmine {

auto GuiObject::add_gui_rect(
    std::vector<Vertex>* buffer, glm::vec2 pos, glm::vec2 size
) -> void {
    buffer->emplace_back(
        pos + 0.5f * glm::vec2{-size.x, -size.y}, glm::vec2{0.0f, 0.0f}
    );
    buffer->emplace_back(
        pos + 0.5f * glm::vec2{-size.x, size.y}, glm::vec2{0.0f, 1.0f}
    );
    buffer->emplace_back(
        pos + 0.5f * glm::vec2{size.x, size.y}, glm::vec2{1.0f, 1.0f}
    );
    buffer->emplace_back(
        pos + 0.5f * glm::vec2{-size.x, -size.y}, glm::vec2{0.0f, 0.0f}
    );
    buffer->emplace_back(
        pos + 0.5f * glm::vec2{size.x, -size.y}, glm::vec2{1.0f, 0.0f}
    );
    buffer->emplace_back(
        pos + 0.5f * glm::vec2{size.x, size.y}, glm::vec2{1.0f, 1.0f}
    );
}

Sprite::Sprite(glm::vec2 pos, f32 size, Texture texture)
: mesh{}
, texture{std::move(texture)}
, pos{pos}
, size{size} {
    auto const aspect_ratio = this->texture.get_aspect_ratio();
    auto& buffer = this->mesh.get_buffer();

    Sprite::add_gui_rect(&buffer, pos, glm::vec2{aspect_ratio * size, size});

    this->mesh.reload_buffer();
}

auto Sprite::render(ShaderProgram const& shader, glm::uvec2 viewport_size)
    -> void {
    auto const aspect_ratio = Window::aspect_ratio_of(viewport_size);
    auto const projection =
        glm::ortho(-aspect_ratio, aspect_ratio, -1.0f, 1.0f, 0.0f, 100.0f);

    this->texture.bind(0);

    shader.bind();
    shader.uniform_mat4("model_projection", projection);

    this->mesh.draw();
}

}  // namespace tmine
