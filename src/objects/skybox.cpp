#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>

#include "../objects.hpp"
#include "../loaders.hpp"
#include "../window.hpp"

using namespace tmine;

Skybox::Skybox(char const* texture_path)
: mesh{Primitive::Triangles}
, shader{load_shader("skybox_vertex.glsl", "skybox_fragment.glsl")}
, texture{Texture::from_image(
      load_png(std::move(texture_path)), TextureLoad::DEFAULT
  )} {
    auto constexpr VERTICES = std::array{
        Skybox::Vertex{glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec2{0.0f, 0.0f}},
        Skybox::Vertex{glm::vec3{-0.5f, 0.5f, -0.5f}, glm::vec2{0.0f, 0.5f}},
        Skybox::Vertex{glm::vec3{0.5f, 0.5f, -0.5f}, glm::vec2{1.0f / 3.0f, 0.5f}},
        Skybox::Vertex{glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec2{0.0f, 0.0f}},
        Skybox::Vertex{glm::vec3{0.5f, -0.5f, -0.5f}, glm::vec2{1.0f / 3.0f, 0.0f}},
        Skybox::Vertex{glm::vec3{0.5f, 0.5f, -0.5f}, glm::vec2{1.0f / 3.0f, 0.5f}},
        Skybox::Vertex{glm::vec3{-0.5f, -0.5f, 0.5f}, glm::vec2{2.0f / 3.0f, 0.5f}},
        Skybox::Vertex{glm::vec3{-0.5f, 0.5f, 0.5f}, glm::vec2{2.0f / 3.0f, 1.0f}},
        Skybox::Vertex{glm::vec3{0.5f, 0.5f, 0.5f}, glm::vec2{1.0f, 1.0f}},
        Skybox::Vertex{glm::vec3{-0.5f, -0.5f, 0.5f}, glm::vec2{2.0f / 3.0f, 0.5f}},
        Skybox::Vertex{glm::vec3{0.5f, -0.5f, 0.5f}, glm::vec2{1.0f, 0.5f}},
        Skybox::Vertex{glm::vec3{0.5f, 0.5f, 0.5f}, glm::vec2{1.0f, 1.0f}},
        Skybox::Vertex{glm::vec3{-0.5f, 0.5f, -0.5f}, glm::vec2{1.0f / 3.0f, 0.0f}},
        Skybox::Vertex{glm::vec3{-0.5f, 0.5f, 0.5f}, glm::vec2{1.0f / 3.0f, 0.5f}},
        Skybox::Vertex{glm::vec3{0.5f, 0.5f, 0.5f}, glm::vec2{2.0f / 3.0f, 0.5f}},
        Skybox::Vertex{glm::vec3{-0.5f, 0.5f, -0.5f}, glm::vec2{1.0f / 3.0f, 0.0f}},
        Skybox::Vertex{glm::vec3{0.5f, 0.5f, -0.5f}, glm::vec2{2.0f / 3.0f, 0.0f}},
        Skybox::Vertex{glm::vec3{0.5f, 0.5f, 0.5f}, glm::vec2{2.0f / 3.0f, 0.5f}},
        Skybox::Vertex{glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec2{0.0f, 0.5f}},
        Skybox::Vertex{glm::vec3{-0.5f, -0.5f, 0.5f}, glm::vec2{0.0f, 1.0f}},
        Skybox::Vertex{glm::vec3{0.5f, -0.5f, 0.5f}, glm::vec2{1.0f / 3.0f, 1.0f}},
        Skybox::Vertex{glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec2{0.0f, 0.5f}},
        Skybox::Vertex{glm::vec3{0.5f, -0.5f, -0.5f}, glm::vec2{1.0f / 3.0f, 0.5f}},
        Skybox::Vertex{glm::vec3{0.5f, -0.5f, 0.5f}, glm::vec2{1.0f / 3.0f, 1.0f}},
        Skybox::Vertex{glm::vec3{0.5f, -0.5f, -0.5f}, glm::vec2{1.0f / 3.0f, 0.5f}},
        Skybox::Vertex{glm::vec3{0.5f, -0.5f, 0.5f}, glm::vec2{1.0f / 3.0f, 1.0f}},
        Skybox::Vertex{glm::vec3{0.5f, 0.5f, 0.5f}, glm::vec2{2.0f / 3.0f, 1.0f}},
        Skybox::Vertex{glm::vec3{0.5f, -0.5f, -0.5f}, glm::vec2{1.0f / 3.0f, 0.5f}},
        Skybox::Vertex{glm::vec3{0.5f, 0.5f, -0.5f}, glm::vec2{2.0f / 3.0f, 0.5f}},
        Skybox::Vertex{glm::vec3{0.5f, 0.5f, 0.5f}, glm::vec2{2.0f / 3.0f, 1.0f}},
        Skybox::Vertex{glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec2{2.0f / 3.0f, 0.0f}},
        Skybox::Vertex{glm::vec3{-0.5f, -0.5f, 0.5f}, glm::vec2{2.0f / 3.0f, 0.5f}},
        Skybox::Vertex{glm::vec3{-0.5f, 0.5f, 0.5f}, glm::vec2{1.0f, 0.5f}},
        Skybox::Vertex{glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec2{2.0f / 3.0f, 0.0f}},
        Skybox::Vertex{glm::vec3{-0.5f, 0.5f, -0.5f}, glm::vec2{1.0f, 0.0f}},
        Skybox::Vertex{glm::vec3{-0.5f, 0.5f, 0.5f}, glm::vec2{1.0f, 0.5f}},
    };

    auto& buffer = this->mesh.get_buffer();
    buffer.insert(buffer.end(), VERTICES.begin(), VERTICES.end());

    this->mesh.reload_buffer();
}

void Skybox::render(
    Camera const& cam, SceneParameters const&, glm::uvec2 window_size
) {
    this->texture.bind(0);
    this->shader.bind();

    this->shader.uniform_mat4(
        "projView", cam.get_projection(Window::aspect_ratio_of(window_size)) *
                        cam.get_view()
    );
    this->shader.uniform_vec3("camPos", cam.get_pos());
    this->shader.uniform_vec2("resolution", glm::vec2{window_size});

    this->mesh.draw();
}
