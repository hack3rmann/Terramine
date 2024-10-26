#include "LineBatch.h"

#include "../graphics.hpp"
#include "../loaders.hpp"

#define LB_VERTEX_SIZE (3 + 4)

using namespace tmine;

LineBatch::LineBatch(usize capacity)
: mesh{{}, LineBatch::VERTEX_ATTRIBUTE_SIZES, Primitive::Lines}
, shader{load_shader("linesVertex.glsl", "linesFragment.glsl").value()} {
    this->mesh.get_buffer().reserve(capacity * LB_VERTEX_SIZE * 2);
}

auto LineBatch::line(
    this LineBatch& self, float x1, float y1, float z1, float x2, float y2,
    float z2, float r, float g, float b, float a
) -> void {
    auto const next_vertices =
        std::array<f32, 14>{x1, y1, z1, r, g, b, a, x2, y2, z2, r, g, b, a};

    auto& buffer = self.mesh.get_buffer();

    buffer.insert(buffer.end(), next_vertices.begin(), next_vertices.end());
}

auto LineBatch::render(this LineBatch& self, Camera const* cam) -> void {
    self.shader.bind();
    self.shader.uniform_mat4("projView", cam->getProjection() * cam->getView());

    self.mesh.get_buffer().resize(LB_VERTEX_SIZE * 2 * 12);

    self.mesh.reload_buffer();
    self.mesh.draw();
}

auto LineBatch::box(
    this LineBatch& self, float x, float y, float z, float w, float h, float d,
    float r, float g, float b, float a
) -> void {
    w *= 0.5f;
    h *= 0.5f;
    d *= 0.5f;

    self.mesh.get_buffer().resize(0);

    self.line(x - w, y - h, z - d, x + w, y - h, z - d, r, g, b, a);
    self.line(x - w, y + h, z - d, x + w, y + h, z - d, r, g, b, a);
    self.line(x - w, y - h, z + d, x + w, y - h, z + d, r, g, b, a);
    self.line(x - w, y + h, z + d, x + w, y + h, z + d, r, g, b, a);

    self.line(x - w, y - h, z - d, x - w, y + h, z - d, r, g, b, a);
    self.line(x + w, y - h, z - d, x + w, y + h, z - d, r, g, b, a);
    self.line(x - w, y - h, z + d, x - w, y + h, z + d, r, g, b, a);
    self.line(x + w, y - h, z + d, x + w, y + h, z + d, r, g, b, a);

    self.line(x - w, y - h, z - d, x - w, y - h, z + d, r, g, b, a);
    self.line(x + w, y - h, z - d, x + w, y - h, z + d, r, g, b, a);
    self.line(x - w, y + h, z - d, x - w, y + h, z + d, r, g, b, a);
    self.line(x + w, y + h, z - d, x + w, y + h, z + d, r, g, b, a);
}
