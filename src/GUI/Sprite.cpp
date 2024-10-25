#include <glm/ext.hpp>

#include "Sprite.h"
#include "../Window.h"
#include "GUI.h"
#include "../loaders.hpp"

using namespace tmine;

Sprite::Sprite() {
    int attrs[] = {2, 2, 4, 0};
    buffer = new float[GUI_VERTEX_SIZE * 6];
    mesh = new Mesh(buffer, 0, attrs);
}

Sprite::Sprite(
    float posX, float posY, float width, float height, Texture texture
)
    : GUIObject(posX, posY, width, height) {
    /* Mesh */
    int attrs[] = {2, 2, 4, 0};
    buffer = new float[GUI_VERTEX_SIZE * 6];
    mesh = new Mesh(buffer, 0, attrs);

    /* Texture */
    this->texture = std::move(texture);

    /* Shader */
    shader = ShaderProgram::from_source(load_shader("GUIVertex.glsl", "GUIFragment.glsl").value()).value();

    /* Buffer */
    int i = 0;
    GUI_RECT(i, posX, posY, width, height, 1.0f, 1.0f, 1.0f, 1.0f);

    /* Init */
    x = posX;
    y = posY;
}

void Sprite::render() {
    /* Texture */
    texture.bind();

    /* Shader */
    shader.bind();

    /* Matrix init */
    float aspect = (float) Window::height / (float) Window::width;
    proj = glm::ortho(-1.0f, 1.0f, -aspect, aspect, 0.0f, 100.0f);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));

    /* Shader uniforms */
    shader.uniform_mat4("modelProj", glm::mat4(1.0f));

    /* Draw */
    mesh->reload(buffer, 6);
    mesh->draw(GL_TRIANGLES);
}

void Sprite::cleanUp() {
    delete mesh;
    delete[] buffer;
}
