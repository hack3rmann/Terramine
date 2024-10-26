#include "Button.h"

#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include "../EventHandler.h"
#include "../Window.h"
#include "GUI.h"
#include "Text.h"
#include "../loaders.hpp"

using namespace tmine;

Button::Button()
: mesh{std::vector<f32>(GUI_VERTEX_SIZE * 6, 0.0f), Button::VERTEX_ATTRIBUTE_SIZES, Primitive::Triangles}
, state{Default} {}

Button::Button(
    float posX, float posY, float width, float height, Texture defTexture,
    Texture hoverTexture, Texture clickedTexture, std::string text,
    std::function<void()> function
)
: GUIObject(posX, posY, width, height)
, function(function)
, mesh{std::vector<f32>(GUI_VERTEX_SIZE * 6, 0.0f), Button::VERTEX_ATTRIBUTE_SIZES, Primitive::Triangles}
, state{Default} {
    textures[Default] = new Texture(std::move(defTexture));
    textures[onHover] = new Texture(std::move(hoverTexture));
    textures[onClick] = new Texture(std::move(clickedTexture));

    shader = load_shader("GUIVertex.glsl", "GUIFragment.glsl").value();

    auto buffer = this->mesh.get_buffer().data();
    int i = 0;
    GUI_RECT(i, posX, posY, width, height, 1.0f, 1.0f, 1.0f, 1.0f);

    /* Coords */
    x = posX;
    y = posY;
    w = width;
    h = height;

    /* Texts */
    this->text = new Text(text, glm::vec2(posX, posY), 1.0f);
}

void Button::render() {
    /* Bind texture */
    textures[state]->bind();

    /* Use shader program */
    shader.bind();

    /* Matrix init */
    float aspect = (float) Window::height / (float) Window::width;
    proj = glm::ortho(-1.0f, 1.0f, -aspect, aspect, 0.0f, 100.0f);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));

    /* Uniforms */
    shader.uniform_mat4("modelProj", glm::mat4(1.0f));

    /* Draw */
    mesh.reload_buffer();
    mesh.draw();
    text->render();
}

void Button::refreshState() {
    float mouseX, mouseY;

    /* Window coords to OpenGL coords */
    mouseX = (Events::x / Window::width - 0.5f) * 2.0f;
    mouseY = -(Events::y / Window::height - 0.5f) * 2.0f;

    /* If mouse inside AABB */
    if (mouseX >= x - w / 2 && mouseX <= x + w / 2 && mouseY >= y - h / 2 &&
        mouseY <= y + h / 2)
    {
        if (Events::isClicked(GLFW_MOUSE_BUTTON_1)) {
            state = onClick;
        } else {
            if (state == onClick) {
                function();
            }

            state = onHover;
        }
    }
    /* If mouse outside AABB */
    else
    {
        if (state == onClick) {
            function();
        }

        state = Default;
    }
}

void Button::cleanUp() {
    for (int i = 0; i < 3; i++) {
        delete textures[i];
    }
}
