#include "../debug.hpp"
#include "../loaders.hpp"
#include "../window.hpp"

namespace tmine {

static auto compactify_color(glm::vec4 color) -> u32 {
    auto compactible_color =
        glm::uvec4(glm::clamp(255.0f * color, 0.0f, 255.0f));
    auto result = u32{0};

    result |= compactible_color.r << 0;
    result |= compactible_color.g << 8;
    result |= compactible_color.b << 16;
    result |= compactible_color.a << 24;

    return result;
}

DebugLines::DebugLines()
: shader{load_shader("debug_lines_vertex.glsl", "debug_lines_fragment.glsl")}
, mesh{Primitive::Lines}
, mutex{} {}

auto DebugLines::line(
    this DebugLines& self, glm::vec3 from, glm::vec3 to, glm::vec4 color
) -> void {
    if (!DEBUG_IS_ENABLED) {
        return;
    }

    auto const compact_color = compactify_color(color);
    auto& buffer = self.mesh.get_buffer();
    buffer.reserve(2);

    buffer.emplace_back(from, compact_color);
    buffer.emplace_back(to, compact_color);
}

auto DebugLines::box(
    this DebugLines& self, glm::vec3 position, glm::vec3 size, glm::vec4 color
) -> void {
    if (!DEBUG_IS_ENABLED) {
        return;
    }

    auto x = position.x;
    auto y = position.y;
    auto z = position.z;
    auto w = size.x;
    auto h = size.y;
    auto d = size.z;

    w *= 0.5f;
    h *= 0.5f;
    d *= 0.5f;

    self.mesh.get_buffer().reserve(24);

    self.line({x - w, y - h, z - d}, {x + w, y - h, z - d}, color);
    self.line({x - w, y + h, z - d}, {x + w, y + h, z - d}, color);
    self.line({x - w, y - h, z + d}, {x + w, y - h, z + d}, color);
    self.line({x - w, y + h, z + d}, {x + w, y + h, z + d}, color);
    self.line({x - w, y - h, z - d}, {x - w, y + h, z - d}, color);
    self.line({x + w, y - h, z - d}, {x + w, y + h, z - d}, color);
    self.line({x - w, y - h, z + d}, {x - w, y + h, z + d}, color);
    self.line({x + w, y - h, z + d}, {x + w, y + h, z + d}, color);
    self.line({x - w, y - h, z - d}, {x - w, y - h, z + d}, color);
    self.line({x + w, y - h, z - d}, {x + w, y - h, z + d}, color);
    self.line({x - w, y + h, z - d}, {x - w, y + h, z + d}, color);
    self.line({x + w, y + h, z - d}, {x + w, y + h, z + d}, color);
}

auto DebugLines::box(this DebugLines& self, Aabb box, glm::vec4 color) -> void {
    self.box(box.center(), box.size(), color);
}

auto DebugLines::render(
    this DebugLines& self, Camera const& cam, glm::uvec2 viewport_size
) -> void {
    if (!DEBUG_IS_ENABLED) {
        return;
    }

    auto const aspect_ratio = Window::aspect_ratio_of(viewport_size);

    // Make sure that depth test is disabled
    glDisable(GL_DEPTH_TEST);

    self.shader.bind();
    self.shader.uniform_mat4(
        "projection_view", cam.get_projection(aspect_ratio) * cam.get_view()
    );
    self.mesh.reload_buffer();
    self.mesh.draw();
    self.mesh.get_buffer().clear();
}

}  // namespace tmine
