#pragma once

#include <glm/glm.hpp>

#include "types.hpp"
#include "graphics.hpp"
#include "window.hpp"
#include "collections.hpp"

namespace tmine {

struct GuiRect {
    glm::vec2 lo;
    glm::vec2 hi;
};

class GuiObject {
public:
    virtual ~GuiObject() = default;

    virtual auto render(ShaderProgram const& shader, glm::uvec2 viewport_size)
        -> void = 0;

    struct Vertex {
        glm::vec2 pos;
        glm::vec2 uv;

        static auto constexpr ATTRIBUTE_SIZES = std::array<usize, 2>{2, 2};
    };

    static auto add_gui_rect(
        RefMut<std::vector<Vertex>> buffer, glm::vec2 pos, glm::vec2 size
    ) -> void;
};

class Sprite : public GuiObject {
public:
    Sprite(glm::vec2 pos, f32 size, Texture texture);

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
        std::shared_ptr<Font> font, Texture glyph_texture,
        std::string_view text, glm::vec2 pos, f32 size
    );

    auto set_text(this Text& self, std::string_view text) -> void;

    auto render(ShaderProgram const& shader, glm::uvec2 viewport_size)
        -> void override;

    inline auto set_position(this Text& self, glm::vec2 pos) -> void {
        self.pos = pos;
    }

    inline auto get_width(this Text const& self) -> f32 {
        return self.width;
    }

private:
    Mesh<GuiObject::Vertex> mesh;
    std::shared_ptr<Font> font;
    Texture glyph_texture;
    glm::vec2 pos;
    f32 size;
    f32 width;
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
    friend class Gui;

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

class GuiStage {
public:
    explicit GuiStage(std::shared_ptr<Font> font);

    auto add_sprite(this GuiStage& self, Sprite sprite) -> void;
    auto add_button(
        this GuiStage& self, StaticString text, glm::vec2 pos, f32 size
    ) -> void;

    auto get_button(this GuiStage const& self, std::string_view name)
        -> Button const&;

    auto render(this GuiStage& self, glm::uvec2 viewport_size) -> void;

    auto update(this GuiStage& self, glm::uvec2 viewport_size) -> void;

private:
    std::unordered_map<StaticString, Button> buttons{};
    std::vector<Sprite> sprites{};
    ButtonStyle button_style;
    std::shared_ptr<Font> font;
    ShaderProgram shader;
};

enum class GuiState {
    InGame = 0,
    StartMenu,
    PauseMenu,
};

class Gui {
public:
    inline Gui()
    : Gui{GuiState::StartMenu} {}

    explicit Gui(GuiState initial_state);

    inline auto current(this Gui const& self) -> GuiState { return self.state; }

    inline auto set_state(this Gui& self, GuiState state) -> void {
        self.state = state;
    }

    auto render(this Gui& self, glm::uvec2 viewport_size) -> void;
    auto update(this Gui& self, RefMut<Window> window) -> void;

    auto add_stage(this Gui& self, GuiState state) -> GuiStage&;
    inline auto get_font(this Gui& self) -> std::shared_ptr<Font> {
        return self.font;
    }

private:
    GuiState state{GuiState::StartMenu};
    std::unordered_map<GuiState, GuiStage> guis;
    std::shared_ptr<Font> font;
};

}  // namespace tmine
