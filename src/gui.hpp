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
        const -> void = 0;

    struct Vertex {
        glm::vec2 pos;
        glm::vec2 uv;

        static auto constexpr ATTRIBUTE_SIZES = std::array<usize, 2>{2, 2};
    };

    static auto add_gui_rect(Mesh<Vertex>* mesh, glm::vec2 pos, glm::vec2 size)
        -> void;
};

class Sprite : public GuiObject {
public:
    Sprite(glm::vec2 pos, f32 size, Texture texture);
    auto render(ShaderProgram const& shader, glm::uvec2 viewport_size) const
        -> void override;

private:
    Mesh<Vertex> mesh;
    Texture texture;
    glm::vec2 pos;
    f32 size;
};


}  // namespace tmine
