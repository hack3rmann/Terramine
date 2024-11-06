#pragma once

#include <glm/glm.hpp>

#include "types.hpp"
#include "graphics.hpp"

namespace tmine {

struct GuiRect {
    glm::vec2 lo;
    glm::vec2 hi;
};

class GuiObject {
public:
    virtual auto render(ShaderProgram const& shader, glm::uvec2 viewport_size)
        -> void = 0;

    struct Vertex {
        glm::vec2 pos;
        glm::vec2 uv;

        static auto constexpr ATTRIBUTE_SIZES = std::array<usize, 2>{2, 2};
    };

    static auto add_gui_rect(
        std::vector<Vertex>* mesh, glm::vec2 pos, glm::vec2 size
    ) -> void;
};

class Sprite : public GuiObject {
public:
    Sprite(glm::vec2 pos, f32 size, Texture texture);
    
    virtual ~Sprite() = default;

    auto render(ShaderProgram const& shader, glm::uvec2 viewport_size)
        -> void override;

private:
    Mesh<GuiObject::Vertex> mesh;
    Texture texture;
    glm::vec2 pos;
    f32 size;
};

class Text : public GuiObject {
public:
    Text(
        Font const& font, Texture glyph_texture, std::string text,
        glm::vec2 pos, f32 size
    );

    virtual ~Text() = default;

    auto render(ShaderProgram const& shader, glm::uvec2 viewport_size)
        -> void override;

private:
    Mesh<GuiObject::Vertex> mesh;
    Texture glyph_texture;
    std::string text;
    glm::vec2 pos;
    f32 size;
};

enum class ButtonState {
    Default = 0,
    Hovered = 1,
    Clicked = 2,
};

struct ButtonStyle {
    std::array<Texture, 3> textures;
    Texture glyph_texture;
};

class Button : public GuiObject {
public:
    Button(ButtonStyle style, Text text, glm::vec2 pos, f32 size);

    virtual ~Button() = default;

    auto render(ShaderProgram const& shader, glm::uvec2 viewport_size)
        -> void override;

    inline auto get_state(this Button const& self) -> ButtonState {
        return self.state;
    }

    inline auto clicked(this Button const& self) -> bool {
        return ButtonState::Clicked == self.state;
    }

    inline auto hovered(this Button const& self) -> bool {
        return ButtonState::Hovered == self.state;
    }

    auto get_size(this Button const& self) -> glm::vec2;

    auto update_state(this Button& self, glm::uvec2 viewport_size) -> void;

private:
    Mesh<GuiObject::Vertex> mesh{};
    ButtonStyle style;
    Text text;
    ButtonState state{ButtonState::Default};
    glm::vec2 pos;
    f32 size;
};

}  // namespace tmine
