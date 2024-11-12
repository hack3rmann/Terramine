#include "../objects.hpp"
#include "../panic.hpp"
#include "../Player.h"

namespace tmine {

char constexpr FRAMEBUFFER_VERTEX_SHADER_NAME[] = "postproc_vertex.glsl";
char constexpr FRAMEBUFFER_FRAGMENT_SHADER_NAME[] = "postproc_fragment.glsl";

Scene::Scene(glm::uvec2 viewport_size)
: frame_buffer{FRAMEBUFFER_VERTEX_SHADER_NAME, FRAMEBUFFER_FRAGMENT_SHADER_NAME, viewport_size}
, viewport_size{viewport_size}
, objects{} {
    this->objects.emplace_back(
        std::make_unique<Skybox>("assets/images/Skybox4.png")
    );
    this->objects.emplace_back(std::make_unique<Terrain>(glm::uvec3{16, 4, 16})
    );
    this->objects.emplace_back(std::make_unique<LineBox>());
}

auto Scene::render(
    this Scene& self, Camera const& camera, glm::uvec2 viewport_size
) -> void {
    // reload frame buffer if viewport size have been updated
    if (viewport_size != self.viewport_size) {
        self.viewport_size = viewport_size;
        self.frame_buffer = FrameBuffer{
            FRAMEBUFFER_VERTEX_SHADER_NAME, FRAMEBUFFER_FRAGMENT_SHADER_NAME,
            viewport_size
        };
    }

    self.frame_buffer.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // FIXME(hack3rmann): remove this check after `FrameBuffer` refactoring
    if (!self.frame_buffer.check()) {
        throw Panic("failed to render to incomplete frame buffer");
    }

    for (auto& object : self.objects) {
        object->render(camera, self.params, viewport_size);
    }

    self.frame_buffer.unbind();

    // Deferred render
    self.frame_buffer.draw();
}

auto Scene::update_player(this Scene& self, void* player) -> void {
    auto terrain = (Terrain*) nullptr;

    if (terrain = dynamic_cast<Terrain*>(self.objects[1].get());
        nullptr == terrain)
    {
        throw Panic("terrain is not found");
    }

    auto line_box = (LineBox*) nullptr;

    if (line_box = dynamic_cast<LineBox*>(self.objects[2].get());
        nullptr == line_box)
    {
        throw Panic("line box is not found");
    }

    ((Player*) player)->update(terrain, line_box, self.viewport_size);
}

}  // namespace tmine
