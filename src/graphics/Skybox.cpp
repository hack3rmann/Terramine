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

Skybox::Skybox(char const* name) {
    /* Init */
    int attrs[] = {3, 2, 0};
    texture = Texture::from_image(load_png(name).value(), TextureLoad::DEFAULT);
    shader = ShaderProgram::from_source(load_shader("SkyboxVertex.glsl", "SkyboxFragment.glsl").value()).value();
    buffer = new float[6 * 6 * SB_VERTEX_SIZE];

    /* Load to buffer */
    int I = 0;
    SB_BOX(I);
    index = I;

    mesh = new Mesh(buffer, 36, attrs);
}

void Skybox::render(Camera const* cam) {
    /* Texture */
    texture.bind();

    /* Shader */
    shader.bind();

    /* Shader uniforms */
    shader.uniform_mat4("projView", cam->getProjection() * cam->getView());
    shader.uniform_vec3("camPos", cam->position);
    shader.uniform_vec2("resolution", vec2(Window::width, Window::height));

    /* Draw */
    mesh->reload(buffer, 36);
    mesh->draw(GL_TRIANGLES);
}
