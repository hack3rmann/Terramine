#include "Button.h"

#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include "GUI.h"
#include "Text.h"
#include "../events.hpp"
#include "../loaders.hpp"
#include "../window.hpp"

using namespace tmine;

Button::Button()
: mesh{std::vector<GUIObject::Vertex>(6), Primitive::Triangles}
, state{Default}
, text{"", glm::vec2{}, 1.0f} {}

Button::Button(
    float posX, float posY, float width, float height, Texture defTexture,
    Texture hoverTexture, Texture clickedTexture, std::string text,
    std::function<void()> function
)
: GUIObject(posX, posY, width, height)
, function(function)
, mesh{std::vector<GUIObject::Vertex>(6), Primitive::Triangles}
, state{Default}
, text{text, glm::vec2(posX, posY), 1.0f} {
    textures[Default] = Texture(std::move(defTexture));
    textures[onHover] = Texture(std::move(hoverTexture));
    textures[onClick] = Texture(std::move(clickedTexture));

    shader = load_shader("gui_vertex.glsl", "gui_fragment.glsl");

    auto& buffer = this->mesh.get_buffer();
    GUI_RECT(buffer, posX, posY, width, height, 1.0f, 1.0f, 1.0f, 1.0f);

    /* Coords */
    x = posX;
    y = posY;
    w = width;
    h = height;
}

auto Button::get_proj(tmine::f32 aspect_ratio) -> glm::mat4 {
    return glm::ortho(-aspect_ratio, aspect_ratio, -1.0f, 1.0f, 0.0f, 100.0f);
}

void Button::render(f32 aspect_ratio) {
    /* Bind texture */
    textures[state].bind(0);

    /* Use shader program */
    shader.bind();

    /* Matrix init */
    auto const proj = Button::get_proj(aspect_ratio);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));

    /* Uniforms */
    shader.uniform_mat4("modelProj", proj * glm::mat4(1.0f));

    /* Draw */
    mesh.reload_buffer();
    mesh.draw();
    text.render(aspect_ratio);
}

void Button::refreshState(glm::uvec2 window_size) {
    auto const proj = Button::get_proj(Window::aspect_ratio_of(window_size));

    /* Window coords to OpenGL coords */

    auto mouseX = (io.get_mouse_pos().x / window_size.x - 0.5f) * 2.0f;
    auto mouseY = -(io.get_mouse_pos().y / window_size.y - 0.5f) * 2.0f;

    auto pos = glm::inverse(proj) * glm::vec4{mouseX, mouseY, 0.0f, 1.0f};
    mouseX = pos.x;
    mouseY = pos.y;

    /* If mouse inside AABB */
    if (mouseX >= x - w / 2 && mouseX <= x + w / 2 && mouseY >= y - h / 2 &&
        mouseY <= y + h / 2)
    {
        if (io.is_clicked(MouseButton::Left)) {
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
