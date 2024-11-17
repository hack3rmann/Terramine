#include "../objects.hpp"
#include "../graphics.hpp"
#include "../loaders.hpp"
#include "../window.hpp"

namespace tmine {

LineBox::LineBox()
: shader{load_shader("lines_vertex.glsl", "lines_fragment.glsl")}
, mesh{Primitive::Lines} {}

auto LineBox::line(
    this LineBox& self, glm::vec3 from, glm::vec3 to, glm::vec4 color
) -> void {
    auto& buffer = self.mesh.get_buffer();

    buffer.emplace_back(from, color);
    buffer.emplace_back(to, color);
}

auto LineBox::clear(this LineBox& self) -> void {
    self.mesh.get_buffer().clear();
}

auto LineBox::render(
    Camera const& cam, SceneParameters const&, glm::uvec2 viewport_size
) -> void {
    auto const aspect_ratio = Window::aspect_ratio_of(viewport_size);

    glEnable(GL_DEPTH_TEST);

    this->shader.bind();
    this->shader.uniform_mat4(
        "projection_view", cam.get_projection(aspect_ratio) * cam.get_view()
    );
    this->mesh.draw();

    glDisable(GL_DEPTH_TEST);
}

auto LineBox::box(
    this LineBox& self, glm::vec3 pos, glm::vec3 sizes, glm::vec4 color
) -> void {
    auto x = pos.x;
    auto y = pos.y;
    auto z = pos.z;
    auto w = sizes.x;
    auto h = sizes.y;
    auto d = sizes.z;
    auto r = color.r;
    auto g = color.g;
    auto b = color.b;
    auto a = color.a;

    w *= 0.5f;
    h *= 0.5f;
    d *= 0.5f;

    self.clear();

    self.line({x - w, y - h, z - d}, {x + w, y - h, z - d}, {r, g, b, a});
    self.line({x - w, y + h, z - d}, {x + w, y + h, z - d}, {r, g, b, a});
    self.line({x - w, y - h, z + d}, {x + w, y - h, z + d}, {r, g, b, a});
    self.line({x - w, y + h, z + d}, {x + w, y + h, z + d}, {r, g, b, a});
    self.line({x - w, y - h, z - d}, {x - w, y + h, z - d}, {r, g, b, a});
    self.line({x + w, y - h, z - d}, {x + w, y + h, z - d}, {r, g, b, a});
    self.line({x - w, y - h, z + d}, {x - w, y + h, z + d}, {r, g, b, a});
    self.line({x + w, y - h, z + d}, {x + w, y + h, z + d}, {r, g, b, a});
    self.line({x - w, y - h, z - d}, {x - w, y - h, z + d}, {r, g, b, a});
    self.line({x + w, y - h, z - d}, {x + w, y - h, z + d}, {r, g, b, a});
    self.line({x - w, y + h, z - d}, {x - w, y + h, z + d}, {r, g, b, a});
    self.line({x + w, y + h, z - d}, {x + w, y + h, z + d}, {r, g, b, a});

    self.mesh.reload_buffer();
}

}  // namespace tmine
