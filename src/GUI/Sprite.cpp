#include <glm/ext.hpp>

#include "Sprite.h"
#include "../Window.h"
#include "GUI.h"
#include "../loaders.hpp"

using namespace tmine;

Sprite::Sprite()
: mesh{
      std::vector(GUI_VERTEX_SIZE * 6, 0.0f), Sprite::VERTEX_ATTRIBUTE_SIZES,
      Primitive::Triangles
  } {}

Sprite::Sprite(
    float posX, float posY, float width, float height, Texture texture
)
: GUIObject(posX, posY, width, height)
, mesh{std::vector(GUI_VERTEX_SIZE * 6, 0.0f), Sprite::VERTEX_ATTRIBUTE_SIZES, Primitive::Triangles}
, texture{std::move(texture)} {
    shader = load_shader("GUIVertex.glsl", "GUIFragment.glsl").value();

    auto buffer = this->mesh.get_buffer().data();
    int i = 0;
    GUI_RECT(i, posX, posY, width, height, 1.0f, 1.0f, 1.0f, 1.0f);

    /* Init */
    x = posX;
    y = posY;
}

void Sprite::render() {
    /* Texture */
    texture.bind(0);

    /* Shader */
    shader.bind();

    /* Matrix init */
    float aspect = (float) Window::height / (float) Window::width;
    proj = glm::ortho(-1.0f, 1.0f, -aspect, aspect, 0.0f, 100.0f);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));

    /* Shader uniforms */
    shader.uniform_mat4("modelProj", glm::mat4(1.0f));

    /* Draw */
    mesh.reload_buffer();
    mesh.draw();
}

void Sprite::cleanUp() {}
