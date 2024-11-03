#include "../objects.hpp"
#include "../graphics.hpp"
#include "../loaders.hpp"

namespace tmine {

LineBox::LineBox()
: shader{load_shader("lines_vertex.glsl", "lines_fragment.glsl")}
, mesh{{}, LineBox::VERTEX_ATTRIBUTE_SIZES, Primitive::Lines} {}

auto LineBox::line(
    this LineBox& self, glm::vec3 from, glm::vec3 to, glm::vec4 color
) -> void {
    auto const next_vertices =
        std::array<f32, 14>{from.x,  from.y,  from.z,  color.r, color.g,
                            color.b, color.a, to.x,    to.y,    to.z,
                            color.r, color.g, color.b, color.a};

    auto& buffer = self.mesh.get_buffer();

    buffer.insert(buffer.end(), next_vertices.begin(), next_vertices.end());
}

auto LineBox::render(
    this LineBox const& self, Camera const& cam, f32 aspect_ratio
) -> void {
    self.shader.bind();
    self.shader.uniform_mat4(
        "projView", cam.get_projection(aspect_ratio) * cam.get_view()
    );
    self.mesh.draw();
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

    self.mesh.get_buffer().resize(0);

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
