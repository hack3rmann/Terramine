#include "../objects.hpp"
#include "../graphics.hpp"
#include "../loaders.hpp"
#include "../window.hpp"

namespace tmine {

SelectionBox::SelectionBox()
: shader{load_shader("lines_vertex.glsl", "lines_fragment.glsl")}
, mesh{Primitive::Lines} {}

auto SelectionBox::line(
    this SelectionBox& self, glm::vec3 from, glm::vec3 to, glm::vec4 color
) -> void {
    auto& buffer = self.mesh.get_buffer();

    buffer.emplace_back(from, color);
    buffer.emplace_back(to, color);
}

auto SelectionBox::clear(this SelectionBox& self) -> void {
    self.mesh.get_buffer().clear();
}

auto SelectionBox::render(
    Camera const& cam, SceneParameters const&, glm::uvec2 viewport_size
) -> void {
    auto const aspect_ratio = Window::aspect_ratio_of(viewport_size);

    this->shader.bind();
    this->shader.uniform_mat4(
        "projection_view", cam.get_projection(aspect_ratio) * cam.get_view()
    );
    this->mesh.draw();
}

auto SelectionBox::box(
    this SelectionBox& self, Aabb box, glm::vec4 color, SideFlags sides
) -> void {
    self.box(box.center(), box.size(), color, sides);
}

auto SelectionBox::box(
    this SelectionBox& self, glm::vec3 pos, glm::vec3 sizes, glm::vec4 color,
    SideFlags sides
) -> void {
    auto x = pos.x;
    auto y = pos.y;
    auto z = pos.z;
    auto w = sizes.x;
    auto h = sizes.y;
    auto d = sizes.z;

    w *= 0.5f;
    h *= 0.5f;
    d *= 0.5f;

    self.clear();

    if (Side::contains(sides, Side::NEG_Y | Side::NEG_Z)) {
        self.line({x - w, y - h, z - d}, {x + w, y - h, z - d}, color);
    }

    if (Side::contains(sides, Side::POS_Y | Side::NEG_Z)) {
        self.line({x - w, y + h, z - d}, {x + w, y + h, z - d}, color);
    }

    if (Side::contains(sides, Side::POS_Z | Side::NEG_Y)) {
        self.line({x - w, y - h, z + d}, {x + w, y - h, z + d}, color);
    }

    if (Side::contains(sides, Side::POS_Y | Side::POS_Z)) {
        self.line({x - w, y + h, z + d}, {x + w, y + h, z + d}, color);
    }

    if (Side::contains(sides, Side::NEG_X | Side::NEG_Z)) {
        self.line({x - w, y - h, z - d}, {x - w, y + h, z - d}, color);
    }

    if (Side::contains(sides, Side::POS_X | Side::NEG_Z)) {
        self.line({x + w, y - h, z - d}, {x + w, y + h, z - d}, color);
    }

    if (Side::contains(sides, Side::POS_Z | Side::NEG_X)) {
        self.line({x - w, y - h, z + d}, {x - w, y + h, z + d}, color);
    }

    if (Side::contains(sides, Side::POS_X | Side::POS_Z)) {
        self.line({x + w, y - h, z + d}, {x + w, y + h, z + d}, color);
    }

    if (Side::contains(sides, Side::NEG_Y | Side::NEG_X)) {
        self.line({x - w, y - h, z - d}, {x - w, y - h, z + d}, color);
    }

    if (Side::contains(sides, Side::POS_X | Side::NEG_Y)) {
        self.line({x + w, y - h, z - d}, {x + w, y - h, z + d}, color);
    }

    if (Side::contains(sides, Side::NEG_X | Side::POS_Y)) {
        self.line({x - w, y + h, z - d}, {x - w, y + h, z + d}, color);
    }

    if (Side::contains(sides, Side::POS_X | Side::POS_Y)) {
        self.line({x + w, y + h, z - d}, {x + w, y + h, z + d}, color);
    }

    self.mesh.reload_buffer();
}

}  // namespace tmine
