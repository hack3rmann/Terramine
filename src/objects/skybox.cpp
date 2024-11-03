#include <bits/stl_algo.h>
#include <bits/ranges_algo.h>

#include "../objects.hpp"
#include "../loaders.hpp"
#include "../window.hpp"

using namespace tmine;

auto constexpr N_VERTICES = usize{36};
auto constexpr VERTEX_SIZE =
    std::ranges::fold_left(Skybox::VERTEX_ATTRIBUTE_SIZES, 0, std::plus{});

static auto add_vertex(
    std::vector<f32>* buffer_ptr, glm::vec3 pos, glm::vec2 uv
) -> void {
    auto const attributes = std::array<f32, 5>{pos.x, pos.y, pos.z, uv.x, uv.y};
    buffer_ptr->insert(buffer_ptr->end(), attributes.begin(), attributes.end());
}

static auto add_skybox_cube(std::vector<f32>* buffer_ptr) -> void {
    add_vertex(buffer_ptr, {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f});
    add_vertex(buffer_ptr, {-0.5f, 0.5f, -0.5f}, {0.0f, 0.5f});
    add_vertex(buffer_ptr, {0.5f, 0.5f, -0.5f}, {1.0f / 3.0f, 0.5f});
    add_vertex(buffer_ptr, {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f});
    add_vertex(buffer_ptr, {0.5f, -0.5f, -0.5f}, {1.0f / 3.0f, 0.0f});
    add_vertex(buffer_ptr, {0.5f, 0.5f, -0.5f}, {1.0f / 3.0f, 0.5f});
    add_vertex(buffer_ptr, {-0.5f, -0.5f, 0.5f}, {2.0f / 3.0f, 0.5f});
    add_vertex(buffer_ptr, {-0.5f, 0.5f, 0.5f}, {2.0f / 3.0f, 1.0f});
    add_vertex(buffer_ptr, {0.5f, 0.5f, 0.5f}, {1.0f, 1.0f});
    add_vertex(buffer_ptr, {-0.5f, -0.5f, 0.5f}, {2.0f / 3.0f, 0.5f});
    add_vertex(buffer_ptr, {0.5f, -0.5f, 0.5f}, {1.0f, 0.5f});
    add_vertex(buffer_ptr, {0.5f, 0.5f, 0.5f}, {1.0f, 1.0f});
    add_vertex(buffer_ptr, {-0.5f, 0.5f, -0.5f}, {1.0f / 3.0f, 0.0f});
    add_vertex(buffer_ptr, {-0.5f, 0.5f, 0.5f}, {1.0f / 3.0f, 0.5f});
    add_vertex(buffer_ptr, {0.5f, 0.5f, 0.5f}, {2.0f / 3.0f, 0.5f});
    add_vertex(buffer_ptr, {-0.5f, 0.5f, -0.5f}, {1.0f / 3.0f, 0.0f});
    add_vertex(buffer_ptr, {0.5f, 0.5f, -0.5f}, {2.0f / 3.0f, 0.0f});
    add_vertex(buffer_ptr, {0.5f, 0.5f, 0.5f}, {2.0f / 3.0f, 0.5f});
    add_vertex(buffer_ptr, {-0.5f, -0.5f, -0.5f}, {0.0f, 0.5f});
    add_vertex(buffer_ptr, {-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f});
    add_vertex(buffer_ptr, {0.5f, -0.5f, 0.5f}, {1.0f / 3.0f, 1.0f});
    add_vertex(buffer_ptr, {-0.5f, -0.5f, -0.5f}, {0.0f, 0.5f});
    add_vertex(buffer_ptr, {0.5f, -0.5f, -0.5f}, {1.0f / 3.0f, 0.5f});
    add_vertex(buffer_ptr, {0.5f, -0.5f, 0.5f}, {1.0f / 3.0f, 1.0f});
    add_vertex(buffer_ptr, {0.5f, -0.5f, -0.5f}, {1.0f / 3.0f, 0.5f});
    add_vertex(buffer_ptr, {0.5f, -0.5f, 0.5f}, {1.0f / 3.0f, 1.0f});
    add_vertex(buffer_ptr, {0.5f, 0.5f, 0.5f}, {2.0f / 3.0f, 1.0f});
    add_vertex(buffer_ptr, {0.5f, -0.5f, -0.5f}, {1.0f / 3.0f, 0.5f});
    add_vertex(buffer_ptr, {0.5f, 0.5f, -0.5f}, {2.0f / 3.0f, 0.5f});
    add_vertex(buffer_ptr, {0.5f, 0.5f, 0.5f}, {2.0f / 3.0f, 1.0f});
    add_vertex(buffer_ptr, {-0.5f, -0.5f, -0.5f}, {2.0f / 3.0f, 0.0f});
    add_vertex(buffer_ptr, {-0.5f, -0.5f, 0.5f}, {2.0f / 3.0f, 0.5f});
    add_vertex(buffer_ptr, {-0.5f, 0.5f, 0.5f}, {1.0f, 0.5f});
    add_vertex(buffer_ptr, {-0.5f, -0.5f, -0.5f}, {2.0f / 3.0f, 0.0f});
    add_vertex(buffer_ptr, {-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f});
    add_vertex(buffer_ptr, {-0.5f, 0.5f, 0.5f}, {1.0f, 0.5f});
}

Skybox::Skybox(char const* texture_path)
: mesh{{}, Skybox::VERTEX_ATTRIBUTE_SIZES, Primitive::Triangles}
, shader{load_shader("SkyboxVertex.glsl", "SkyboxFragment.glsl")}
, texture{Texture::from_image(
      load_png(std::move(texture_path)), TextureLoad::DEFAULT
  )} {
    auto& buffer = this->mesh.get_buffer();

    buffer.reserve(VERTEX_SIZE * N_VERTICES);
    add_skybox_cube(&buffer);

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
