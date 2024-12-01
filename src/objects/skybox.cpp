#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>

#include "../objects.hpp"
#include "../loaders.hpp"
#include "../window.hpp"

using namespace tmine;

Skybox::Skybox()
: mesh{Primitive::Triangles}
, shader{load_shader("skybox_vertex.glsl", "skybox_fragment.glsl")}
, texture{Texture::from_image(
      load_png("assets/images/Skybox4.png"), TextureLoad::DEFAULT
  )} {
    auto constexpr VERTICES = std::array{
        Skybox::Vertex{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
        Skybox::Vertex{{0.5f, 0.5f, -0.5f}, {1.0f / 3.0f, 0.5f}},
        Skybox::Vertex{{-0.5f, 0.5f, -0.5f}, {0.0f, 0.5f}},
        Skybox::Vertex{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
        Skybox::Vertex{{0.5f, -0.5f, -0.5f}, {1.0f / 3.0f, 0.0f}},
        Skybox::Vertex{{0.5f, 0.5f, -0.5f}, {1.0f / 3.0f, 0.5f}},

        Skybox::Vertex{{-0.5f, -0.5f, 0.5f}, {2.0f / 3.0f, 0.5f}},
        Skybox::Vertex{{-0.5f, 0.5f, 0.5f}, {2.0f / 3.0f, 1.0f}},
        Skybox::Vertex{{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}},
        Skybox::Vertex{{-0.5f, -0.5f, 0.5f}, {2.0f / 3.0f, 0.5f}},
        Skybox::Vertex{{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}},
        Skybox::Vertex{{0.5f, -0.5f, 0.5f}, {1.0f, 0.5f}},

        Skybox::Vertex{{-0.5f, 0.5f, -0.5f}, {1.0f / 3.0f, 0.0f}},
        Skybox::Vertex{{0.5f, 0.5f, 0.5f}, {2.0f / 3.0f, 0.5f}},
        Skybox::Vertex{{-0.5f, 0.5f, 0.5f}, {1.0f / 3.0f, 0.5f}},
        Skybox::Vertex{{-0.5f, 0.5f, -0.5f}, {1.0f / 3.0f, 0.0f}},
        Skybox::Vertex{{0.5f, 0.5f, -0.5f}, {2.0f / 3.0f, 0.0f}},
        Skybox::Vertex{{0.5f, 0.5f, 0.5f}, {2.0f / 3.0f, 0.5f}},

        Skybox::Vertex{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.5f}},
        Skybox::Vertex{{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}},
        Skybox::Vertex{{0.5f, -0.5f, 0.5f}, {1.0f / 3.0f, 1.0f}},
        Skybox::Vertex{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.5f}},
        Skybox::Vertex{{0.5f, -0.5f, 0.5f}, {1.0f / 3.0f, 1.0f}},
        Skybox::Vertex{{0.5f, -0.5f, -0.5f}, {1.0f / 3.0f, 0.5f}},

        Skybox::Vertex{{0.5f, -0.5f, -0.5f}, {1.0f / 3.0f, 0.5f}},
        Skybox::Vertex{{0.5f, -0.5f, 0.5f}, {1.0f / 3.0f, 1.0f}},
        Skybox::Vertex{{0.5f, 0.5f, 0.5f}, {2.0f / 3.0f, 1.0f}},
        Skybox::Vertex{{0.5f, -0.5f, -0.5f}, {1.0f / 3.0f, 0.5f}},
        Skybox::Vertex{{0.5f, 0.5f, 0.5f}, {2.0f / 3.0f, 1.0f}},
        Skybox::Vertex{{0.5f, 0.5f, -0.5f}, {2.0f / 3.0f, 0.5f}},

        Skybox::Vertex{{-0.5f, -0.5f, -0.5f}, {2.0f / 3.0f, 0.0f}},
        Skybox::Vertex{{-0.5f, 0.5f, 0.5f}, {1.0f, 0.5f}},
        Skybox::Vertex{{-0.5f, -0.5f, 0.5f}, {2.0f / 3.0f, 0.5f}},
        Skybox::Vertex{{-0.5f, -0.5f, -0.5f}, {2.0f / 3.0f, 0.0f}},
        Skybox::Vertex{{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}},
        Skybox::Vertex{{-0.5f, 0.5f, 0.5f}, {1.0f, 0.5f}},
    };

    auto& buffer = this->mesh.get_buffer();
    buffer.insert(buffer.end(), VERTICES.begin(), VERTICES.end());

    this->mesh.reload_buffer();
}

void Skybox::render(
    Camera const& cam, SceneParameters const&, RenderPass pass
) {
    glEnable(GL_CULL_FACE);

    this->texture.bind(0);
    this->shader.bind();

    this->shader.uniform_mat4(
        "projection_rotation",
        cam.get_projection(Window::aspect_ratio_of(pass.viewport_size)) *
            glm::transpose(cam.get_rotation())
    );

    this->mesh.draw();

    glDisable(GL_CULL_FACE);
}
