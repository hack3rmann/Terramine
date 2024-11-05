#include <glm/ext.hpp>

#include "Sprite.h"
#include "GUI.h"
#include "../loaders.hpp"

using namespace tmine;

Sprite::Sprite()
: mesh{std::vector<GUIObject::Vertex>(6), Primitive::Triangles} {}

Sprite::Sprite(
    float posX, float posY, f32 size, Texture texture
)
: GUIObject(posX, posY, size, size)
, mesh{std::vector<GUIObject::Vertex>(6), Primitive::Triangles}
, texture{std::move(texture)} {
    shader = load_shader("gui_vertex.glsl", "gui_fragment.glsl");

    auto const texture_size = this->texture.get_size();
    auto const aspect_ratio = (f32) texture_size.y / (f32) texture_size.x;

    auto& buffer = this->mesh.get_buffer();
    GUI_RECT(buffer, posX, posY, size, aspect_ratio * size, 1.0f, 1.0f, 1.0f, 1.0f);

    /* Init */
    x = posX;
    y = posY;
    width = size;
    height = aspect_ratio * size;
}

auto Sprite::get_proj(tmine::f32 aspect_ratio) -> glm::mat4 {
    return glm::ortho(-aspect_ratio, aspect_ratio, -1.0f, 1.0f, 0.0f, 100.0f);
}

void Sprite::render(f32 aspect_ratio) {
    /* Texture */
    texture.bind(0);

    /* Shader */
    shader.bind();

    /* Matrix init */
    auto const proj = Sprite::get_proj(aspect_ratio);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));

    /* Shader uniforms */
    shader.uniform_mat4("modelProj", proj * glm::mat4(1.0f));

    /* Draw */
    mesh.reload_buffer();
    mesh.draw();
}
