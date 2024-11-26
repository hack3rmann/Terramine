#include "../objects.hpp"
#include "../loaders.hpp"

namespace tmine {

char constexpr FRAMEBUFFER_VERTEX_SHADER_NAME[] = "postproc_vertex.glsl";
char constexpr FRAMEBUFFER_FRAGMENT_SHADER_NAME[] = "postproc_fragment.glsl";

Scene::Scene(glm::uvec2 viewport_size)
: deferred_shader{load_shader(
      FRAMEBUFFER_VERTEX_SHADER_NAME, FRAMEBUFFER_FRAGMENT_SHADER_NAME
  )}
, deferred_renderer{this->deferred_shader, viewport_size}
, viewport_size{viewport_size}
, objects{} {
    this->add(Skybox{});
    this->add_unique(Terrain{glm::uvec3{16, 4, 16}});
    this->add_unique(SelectionBox{});
}

auto Scene::render(
    this Scene& self, Camera const& camera, glm::uvec2 viewport_size
) -> void {
    // reload frame buffer if viewport size have been updated
    if (viewport_size != self.viewport_size) {
        self.viewport_size = viewport_size;
        self.deferred_renderer =
            DeferredRenderer{self.deferred_shader, viewport_size};
    }

    self.deferred_renderer.bind_geometry_buffer();
    self.deferred_renderer.clear();

    for (auto& object : self.objects) {
        object->render(
            camera, self.params,
            RenderPass{
                .viewport_size = viewport_size,
                .ssaa_level = self.deferred_renderer.ssaa_level(),
            }
        );
    }

    self.deferred_renderer.unbind_geometry_buffer();
    self.deferred_renderer.draw_screen_pass();
}

}  // namespace tmine
