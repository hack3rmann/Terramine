#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>

#include "../objects.hpp"
#include "../loaders.hpp"
#include "../window.hpp"

using namespace tmine;

Skybox::Skybox(char const* texture_path)
: mesh{{}, Primitive::Triangles}
, shader{load_shader("skybox_vertex.glsl", "skybox_fragment.glsl")}
, texture{Texture::from_image(
      load_png(std::move(texture_path)), TextureLoad::DEFAULT
  )} {
    auto& buffer = this->mesh.get_buffer();

    auto constexpr N_VERTICES = usize{36};
    auto constexpr VERTEX_SIZE =
        std::ranges::fold_left(Skybox::Vertex::ATTRIBUTE_SIZES, 0, std::plus{});

    buffer.reserve(VERTEX_SIZE * N_VERTICES);

    buffer.emplace_back(glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec2{0.0f, 0.0f});
    buffer.emplace_back(glm::vec3{-0.5f, 0.5f, -0.5f}, glm::vec2{0.0f, 0.5f});
    buffer.emplace_back(glm::vec3{0.5f, 0.5f, -0.5f}, glm::vec2{1.0f / 3.0f, 0.5f});
    buffer.emplace_back(glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec2{0.0f, 0.0f});
    buffer.emplace_back(glm::vec3{0.5f, -0.5f, -0.5f}, glm::vec2{1.0f / 3.0f, 0.0f});
    buffer.emplace_back(glm::vec3{0.5f, 0.5f, -0.5f}, glm::vec2{1.0f / 3.0f, 0.5f});
    buffer.emplace_back(glm::vec3{-0.5f, -0.5f, 0.5f}, glm::vec2{2.0f / 3.0f, 0.5f});
    buffer.emplace_back(glm::vec3{-0.5f, 0.5f, 0.5f}, glm::vec2{2.0f / 3.0f, 1.0f});
    buffer.emplace_back(glm::vec3{0.5f, 0.5f, 0.5f}, glm::vec2{1.0f, 1.0f});
    buffer.emplace_back(glm::vec3{-0.5f, -0.5f, 0.5f}, glm::vec2{2.0f / 3.0f, 0.5f});
    buffer.emplace_back(glm::vec3{0.5f, -0.5f, 0.5f}, glm::vec2{1.0f, 0.5f});
    buffer.emplace_back(glm::vec3{0.5f, 0.5f, 0.5f}, glm::vec2{1.0f, 1.0f});
    buffer.emplace_back(glm::vec3{-0.5f, 0.5f, -0.5f}, glm::vec2{1.0f / 3.0f, 0.0f});
    buffer.emplace_back(glm::vec3{-0.5f, 0.5f, 0.5f}, glm::vec2{1.0f / 3.0f, 0.5f});
    buffer.emplace_back(glm::vec3{0.5f, 0.5f, 0.5f}, glm::vec2{2.0f / 3.0f, 0.5f});
    buffer.emplace_back(glm::vec3{-0.5f, 0.5f, -0.5f}, glm::vec2{1.0f / 3.0f, 0.0f});
    buffer.emplace_back(glm::vec3{0.5f, 0.5f, -0.5f}, glm::vec2{2.0f / 3.0f, 0.0f});
    buffer.emplace_back(glm::vec3{0.5f, 0.5f, 0.5f}, glm::vec2{2.0f / 3.0f, 0.5f});
    buffer.emplace_back(glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec2{0.0f, 0.5f});
    buffer.emplace_back(glm::vec3{-0.5f, -0.5f, 0.5f}, glm::vec2{0.0f, 1.0f});
    buffer.emplace_back(glm::vec3{0.5f, -0.5f, 0.5f}, glm::vec2{1.0f / 3.0f, 1.0f});
    buffer.emplace_back(glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec2{0.0f, 0.5f});
    buffer.emplace_back(glm::vec3{0.5f, -0.5f, -0.5f}, glm::vec2{1.0f / 3.0f, 0.5f});
    buffer.emplace_back(glm::vec3{0.5f, -0.5f, 0.5f}, glm::vec2{1.0f / 3.0f, 1.0f});
    buffer.emplace_back(glm::vec3{0.5f, -0.5f, -0.5f}, glm::vec2{1.0f / 3.0f, 0.5f});
    buffer.emplace_back(glm::vec3{0.5f, -0.5f, 0.5f}, glm::vec2{1.0f / 3.0f, 1.0f});
    buffer.emplace_back(glm::vec3{0.5f, 0.5f, 0.5f}, glm::vec2{2.0f / 3.0f, 1.0f});
    buffer.emplace_back(glm::vec3{0.5f, -0.5f, -0.5f}, glm::vec2{1.0f / 3.0f, 0.5f});
    buffer.emplace_back(glm::vec3{0.5f, 0.5f, -0.5f}, glm::vec2{2.0f / 3.0f, 0.5f});
    buffer.emplace_back(glm::vec3{0.5f, 0.5f, 0.5f}, glm::vec2{2.0f / 3.0f, 1.0f});
    buffer.emplace_back(glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec2{2.0f / 3.0f, 0.0f});
    buffer.emplace_back(glm::vec3{-0.5f, -0.5f, 0.5f}, glm::vec2{2.0f / 3.0f, 0.5f});
    buffer.emplace_back(glm::vec3{-0.5f, 0.5f, 0.5f}, glm::vec2{1.0f, 0.5f});
    buffer.emplace_back(glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec2{2.0f / 3.0f, 0.0f});
    buffer.emplace_back(glm::vec3{-0.5f, 0.5f, -0.5f}, glm::vec2{1.0f, 0.0f});
    buffer.emplace_back(glm::vec3{-0.5f, 0.5f, 0.5f}, glm::vec2{1.0f, 0.5f});

    this->mesh.reload_buffer();
}

void Skybox::render(
    this Skybox const& self, Camera const& cam, glm::uvec2 window_size
) {
    self.texture.bind(0);
    self.shader.bind();

    self.shader.uniform_mat4(
        "projView",
        cam.get_projection(Window::aspect_ratio_of(window_size)) * cam.get_view()
    );
    self.shader.uniform_vec3("camPos", cam.get_pos());
    self.shader.uniform_vec2("resolution", glm::vec2{window_size});

    self.mesh.draw();
}
