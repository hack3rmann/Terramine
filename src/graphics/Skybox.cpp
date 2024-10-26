#include <array>

#include "Skybox.h"
#include "../Window.h"
#include "../loaders.hpp"

#define SB_VERTEX_SIZE (3 + 2)
#define SB_VERTEX(I, X, Y, Z, T, S) \
    buffer[0 + I] = X;              \
    buffer[1 + I] = Y;              \
    buffer[2 + I] = Z;              \
    buffer[3 + I] = T;              \
    buffer[4 + I] = S;              \
    I += SB_VERTEX_SIZE;
#define SB_BOX(I)                                                   \
    SB_VERTEX(I, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f) /* BACK */        \
    SB_VERTEX(I, -0.5f, 0.5f, -0.5f, 0.0f, 0.5f)                    \
    SB_VERTEX(I, 0.5f, 0.5f, -0.5f, 1.0f / 3.0f, 0.5f)              \
    SB_VERTEX(I, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f)                   \
    SB_VERTEX(I, 0.5f, -0.5f, -0.5f, 1.0f / 3.0f, 0.0f)             \
    SB_VERTEX(I, 0.5f, 0.5f, -0.5f, 1.0f / 3.0f, 0.5f)              \
    SB_VERTEX(I, -0.5f, -0.5f, 0.5f, 2.0f / 3.0f, 0.5f) /* FRONT */ \
    SB_VERTEX(I, -0.5f, 0.5f, 0.5f, 2.0f / 3.0f, 1.0f)              \
    SB_VERTEX(I, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f)                      \
    SB_VERTEX(I, -0.5f, -0.5f, 0.5f, 2.0f / 3.0f, 0.5f)             \
    SB_VERTEX(I, 0.5f, -0.5f, 0.5f, 1.0f, 0.5f)                     \
    SB_VERTEX(I, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f)                      \
    SB_VERTEX(I, -0.5f, 0.5f, -0.5f, 1.0f / 3.0f, 0.0f) /* TOP */   \
    SB_VERTEX(I, -0.5f, 0.5f, 0.5f, 1.0f / 3.0f, 0.5f)              \
    SB_VERTEX(I, 0.5f, 0.5f, 0.5f, 2.0f / 3.0f, 0.5f)               \
    SB_VERTEX(I, -0.5f, 0.5f, -0.5f, 1.0f / 3.0f, 0.0f)             \
    SB_VERTEX(I, 0.5f, 0.5f, -0.5f, 2.0f / 3.0f, 0.0f)              \
    SB_VERTEX(I, 0.5f, 0.5f, 0.5f, 2.0f / 3.0f, 0.5f)               \
    SB_VERTEX(I, -0.5f, -0.5f, -0.5f, 0.0f, 0.5f) /* BOTTOM */      \
    SB_VERTEX(I, -0.5f, -0.5f, 0.5f, 0.0f, 1.0f)                    \
    SB_VERTEX(I, 0.5f, -0.5f, 0.5f, 1.0f / 3.0f, 1.0f)              \
    SB_VERTEX(I, -0.5f, -0.5f, -0.5f, 0.0f, 0.5f)                   \
    SB_VERTEX(I, 0.5f, -0.5f, -0.5f, 1.0f / 3.0f, 0.5f)             \
    SB_VERTEX(I, 0.5f, -0.5f, 0.5f, 1.0f / 3.0f, 1.0f)              \
    SB_VERTEX(I, 0.5f, -0.5f, -0.5f, 1.0f / 3.0f, 0.5f) /* RIGHT */ \
    SB_VERTEX(I, 0.5f, -0.5f, 0.5f, 1.0f / 3.0f, 1.0f)              \
    SB_VERTEX(I, 0.5f, 0.5f, 0.5f, 2.0f / 3.0f, 1.0f)               \
    SB_VERTEX(I, 0.5f, -0.5f, -0.5f, 1.0f / 3.0f, 0.5f)             \
    SB_VERTEX(I, 0.5f, 0.5f, -0.5f, 2.0f / 3.0f, 0.5f)              \
    SB_VERTEX(I, 0.5f, 0.5f, 0.5f, 2.0f / 3.0f, 1.0f)               \
    SB_VERTEX(I, -0.5f, -0.5f, -0.5f, 2.0f / 3.0f, 0.0f) /* LEFT */ \
    SB_VERTEX(I, -0.5f, -0.5f, 0.5f, 2.0f / 3.0f, 0.5f)             \
    SB_VERTEX(I, -0.5f, 0.5f, 0.5f, 1.0f, 0.5f)                     \
    SB_VERTEX(I, -0.5f, -0.5f, -0.5f, 2.0f / 3.0f, 0.0f)            \
    SB_VERTEX(I, -0.5f, 0.5f, -0.5f, 1.0f, 0.0f)                    \
    SB_VERTEX(I, -0.5f, 0.5f, 0.5f, 1.0f, 0.5f)

using namespace tmine;

Skybox::Skybox(char const* name)
: shader{load_shader("SkyboxVertex.glsl", "SkyboxFragment.glsl").value()}
, texture{Texture::from_image(load_png(name).value(), TextureLoad::DEFAULT)}
, mesh{{}, Skybox::VERTEX_ATTRIBUTE_SIZES, Primitive::Triangles} {
    auto& buffer = this->mesh.get_buffer();
    buffer.resize(6 * 6 * SB_VERTEX_SIZE);

    usize i = 0;
    SB_BOX(i);

    this->mesh.reload_buffer();
}

void Skybox::render(Camera const* cam) {
    texture.bind();
    shader.bind();

    shader.uniform_mat4("projView", cam->getProjection() * cam->getView());
    shader.uniform_vec3("camPos", cam->position);
    shader.uniform_vec2("resolution", vec2(Window::width, Window::height));

    mesh.draw();
}
