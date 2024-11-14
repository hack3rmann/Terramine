#include "../panic.hpp"
#include "../graphics.hpp"

namespace tmine {

DeferredRenderer::DeferredRenderer(
    ShaderProgram shader, glm::uvec2 viewport_size
)
: geometry_buffer{viewport_size}
, shader{std::move(shader)}
, mesh{Primitive::Triangles} {
    auto& buffer = this->mesh.get_buffer();
    buffer.insert(buffer.end(), SCREEN_QUAD.begin(), SCREEN_QUAD.end());

    this->mesh.reload_buffer();
}

auto DeferredRenderer::bind_geometry_buffer(this DeferredRenderer& self)
    -> void {
    self.is_bound = true;
    self.geometry_buffer.bind_frame_buffer();
}

auto DeferredRenderer::unbind_geometry_buffer(this DeferredRenderer& self)
    -> void {
    self.is_bound = false;
    self.geometry_buffer.unbind_frame_buffer();
}

auto DeferredRenderer::draw_screen_pass(this DeferredRenderer const& self)
    -> void {
    self.shader.bind();

    self.shader.uniform_int("screenDepth", 0);
    self.shader.uniform_int("screenColor", 1);
    self.shader.uniform_vec2(
        "resolution", glm::vec2{self.geometry_buffer.get_viewport_size()}
    );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(
        GL_TEXTURE_2D, self.geometry_buffer.data->depth_attachment_id
    );
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(
        GL_TEXTURE_2D, self.geometry_buffer.data->color_attachment_id
    );

    self.mesh.draw();
}

auto DeferredRenderer::clear([[maybe_unused]] this DeferredRenderer const& self)
    -> void {
    if (!self.is_bound) {
        throw Panic("failed to clear unbound geometry buffer");
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

}  // namespace tmine
