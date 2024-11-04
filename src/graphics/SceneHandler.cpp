#include "SceneHandler.h"

#include <GLFW/glfw3.h>

#include "../events.hpp"
#include "../window.hpp"

using namespace tmine;

/* Scene Handler */
SceneHandler::SceneHandler(glm::uvec2 window_size)
: plr{(f32) glfwGetTime(), -30.0f, vec3(0.0f)}
, terrarian{}
, lines{}
, skybox{}
, fb{"postproc_vertex.glsl", "postproc_fragment.glsl", window_size}
, shadowBuff{"postproc_vertex.glsl", "postproc_fragment.glsl", window_size} {}

void SceneHandler::terminate() {
    terrarian.terminate();
    lines.terminate();
    skybox.terminate();
    fb.terminate();
}

void SceneHandler::updateAll(glm::uvec2 window_size) {
    plr.update(&terrarian.terrain, &lines.lineBatch, window_size);
    terrarian.reloadChunks(&plr.cam);
}

void SceneHandler::updateChunks() { terrarian.reloadChunks(&plr.cam); }

void SceneHandler::updatePlayer(glm::uvec2 window_size) {
    plr.update(&terrarian.terrain, &lines.lineBatch, window_size);
}

void SceneHandler::render(glm::uvec2 window_size) {
    /* Viewport change handle */
    if (this->window_size != window_size) {
        this->window_size = window_size;
        fb.reload(
            "postproc_vertex.glsl", "postproc_fragment.glsl", window_size
        );
    }

    if (io.just_pressed(Key::R)) {
        terrarian.refreshRes();
        fb.refreshShader();
    }

    /* Render to shadow view */
    shadowBuff.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    terrarian.render(&plr.cam, this->window_size);
    shadowBuff.unbind();

    /* Render to post-processing framebuffer */
    fb.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!fb.check()) {
        fprintf(stderr, "Frame buffer in not complete...");
    }

    skybox.render(&plr.cam, this->window_size);
    terrarian.renderShadows(&plr.cam, &shadowBuff, this->window_size);
    lines.render(&plr.cam, Window::aspect_ratio_of(window_size));
    fb.unbind();

    /* Draw result of postprocessing */
    fb.drawColor();
}

/* Skybox Handler */
SkyboxHandler::SkyboxHandler() {
    current = 0;
    skyboxes[0] = new Skybox("assets/images/Skybox4.png");
}

void SkyboxHandler::terminate() {
    for (unsigned int i = 0; i < 5; i++) {
        delete skyboxes[i];
    }
}

void SkyboxHandler::render(Camera const* cam, glm::uvec2 window_size) {
    skyboxes[current]->render(*cam, window_size);
}

/* Terrarian handler */
TerrarianHandler::TerrarianHandler()
: terrain{glm::uvec3{16, 4, 16}} {}

void TerrarianHandler::reloadChunks([[maybe_unused]] Camera const* cam) {
    this->terrain.update();
}

void TerrarianHandler::terminate() {}

void TerrarianHandler::render(Camera const* cam, glm::uvec2 window_size) {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    this->terrain.render(*cam, glm::vec3{0.2f, -0.5f, 1.0f}, window_size);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
}

void TerrarianHandler::renderShadows(
    Camera const* cam, [[maybe_unused]] FrameBuffer* shadowBuff,
    glm::uvec2 window_size
) {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    this->terrain.render(*cam, glm::vec3{0.2f, -0.5f, 1.0f}, window_size);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

void TerrarianHandler::refreshRes() {}

/* LineBatch handler */
LineBoxHandler::LineBoxHandler()
: lineBatch{} {}

void LineBoxHandler::terminate() {}

void LineBoxHandler::render(Camera const* cam, f32 aspect_ratio) {
    glEnable(GL_DEPTH_TEST);
    lineBatch.render(*cam, aspect_ratio);
    glDisable(GL_DEPTH_TEST);
}
