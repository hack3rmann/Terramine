#include "Terrarian.h"

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "Chunk.h"
#include "../Window.h"
#include "../loaders.hpp"

using namespace tmine;

mat4 rotation(1.0f);

void Terrarian::render(Camera const* cam) {
    rotation = rotate(rotation, 0.01f, vec3(1.0f, 0.0f, 0.0f));
    shader->use();
    shader->uniformMatrix("proj", cam->getProjection());
    shader->uniformMatrix("view", cam->getView());
    shader->uniformVec2u("resolution", vec2(Window::width, Window::height));
    shader->uniform3f("toLightVec", vec3(rotation * vec4(toLightVec, 1.0f)));
    shader->uniform3f("lightColor", vec3(0.96f, 0.24f, 0.0f));
    // shader->uniform3f("lightColor", vec3(0.73f, 0.54f, 0.95f));
    glActiveTexture(GL_TEXTURE0);
    textureAtlas.bind();
    shader->uniform1i("u_Texture0", 0);
    glActiveTexture(GL_TEXTURE1);
    normalAtlas.bind();
    shader->uniform1i("u_Texture1", 1);
    mat4 model(1.0f);
    for (unsigned long long i = 0; i < chunks->volume; i++) {
        shader->use();
        Chunk* chunk = chunks->chunks[i];
        Mesh* mesh = meshes[i];
        if (mesh == nullptr) {
            continue;
        }
        model = glm::translate(
            mat4(1.0f), vec3(
                            chunk->x * CHUNK_W + 0.5f,
                            chunk->y * CHUNK_H + 0.5f, chunk->z * CHUNK_D + 0.5f
                        )
        );
        shader->uniformMatrix("model", model);
        mesh->draw(GL_TRIANGLES);
    }
    glActiveTexture(GL_TEXTURE0);
}

Terrarian::Terrarian(char const* textureAtlas)
    : renderer(1024 * 1024 * 4) {
    onceLoad = 1;
    this->textureAtlas = Texture::from_image(load_png("assets/textureAtlas3.png").value(), TextureLoad::DEFAULT);
    this->normalAtlas = Texture::from_image(load_png("assets/normalAtlas3.png").value(), TextureLoad::DEFAULT);
    if (textureAtlas == nullptr) {
        fprintf(stderr, "Can not load texture in %s, %d\n", __FILE__, __LINE__);
        delete textureAtlas;
    }
    shader = load_shader("vertexShader.glsl", "fragmentShader.glsl");
    if (shader == nullptr) {
        fprintf(stderr, "Can not load shader in %s, %d\n", __FILE__, __LINE__);
        delete textureAtlas;
        delete shader;
    }

    chunks = new Chunks(8, 8, 8);

    toLightVec = vec3(-0.2f, 0.5f, -1.0f);

    fprintf(stderr, "Creating 2d meshes array...\t");
    meshes = new Mesh*[chunks->volume];
    for (unsigned long long i = 0; i < chunks->volume; i++) {
        meshes[i] = nullptr;
    }
    fprintf(stderr, "DONE\n");
}

Terrarian::~Terrarian() {
    delete shader;
    delete chunks;
}

void Terrarian::reload() {
    int percentage = 0;

    Chunk* closes[27];
    for (unsigned long long i = 0; i < chunks->volume; i++) {
        Chunk* chunk = chunks->chunks[i];
        if (!chunk->modified) {
            continue;
        }
        chunk->modified = false;
        if (meshes[i] != nullptr) {
            delete meshes[i];
        }

        if (percentage !=
                (int) ((float) i / (float) (chunks->volume - 1) * 100.0f) ||
            i == 0)
        {
            percentage =
                (int) ((float) i / (float) (chunks->volume - 1) * 100.0f);
            if (onceLoad) {
                fprintf(stderr, "Reloading chunks...\t%d%%\r", percentage);
            }
        }
        if (i == chunks->volume - 1) {
            if (onceLoad) {
                fputc('\n', stderr);
            }
        }

        for (int i = 0; i < 27; i++) {
            closes[i] = nullptr;
        }
        for (unsigned long long j = 0; j < chunks->volume; j++) {
            Chunk* other = chunks->chunks[j];

            int ox = other->x - chunk->x;
            int oy = other->y - chunk->y;
            int oz = other->z - chunk->z;

            if (abs(ox) > 1 || abs(oy) > 1 || abs(oz) > 1) {
                continue;
            }

            ox += 1;
            oy += 1;
            oz += 1;
            closes[(oy * 3 + oz) * 3 + ox] = other;
        }
        Mesh* mesh = renderer.render(chunk, (Chunk const**) closes, true);
        meshes[i] = mesh;
    }
    onceLoad = 0;
}

void Terrarian::refreshShader() {
    shader = load_shader("vertexShader.glsl", "fragmentShader.glsl");
}

void Terrarian::refreshTextures() {
    textureAtlas = Texture::from_image(load_png("assets/textureAtlas3.png").value(), TextureLoad::DEFAULT);
    normalAtlas = Texture::from_image(load_png("assets/normalAtlas3.png").value(), TextureLoad::DEFAULT);
}
