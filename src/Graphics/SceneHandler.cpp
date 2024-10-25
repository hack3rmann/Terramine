#include "SceneHandler.h"

#include <GLFW/glfw3.h>

#include "../EventHandler.h"

/* Scene Handler */
SceneHandler::SceneHandler() {
    fb = new FrameBuffer("PPVertex.glsl", "PPFragment.glsl");
    plr = new Player(glfwGetTime(), -30.0f, vec3(0.0f));
    lines = new LineBatchHandler();
    skybox = new SkyboxHandler();
    terrarian = new TerrarianHandler();
    shadowBuff = new FrameBuffer("PPVertex.glsl", "PPFragment.glsl");
}

void SceneHandler::terminate() {
    terrarian->terminate();
    lines->terminate();
    skybox->terminate();
    fb->terminate();

    delete plr;
    delete fb;
    delete terrarian;
    delete lines;
    delete skybox;
}

void SceneHandler::updateAll() {
    plr->update(terrarian->terra->chunks, lines->lineBatch);
    terrarian->reloadChunks(plr->cam);
}

void SceneHandler::updateChunks() { terrarian->reloadChunks(plr->cam); }

void SceneHandler::updatePlayer() {
    plr->update(terrarian->terra->chunks, lines->lineBatch);
}

void SceneHandler::render() {
    /* Viewport change handle */
    if (Window::viewPortChange) {
        fb->reload("PPVertex.glsl", "PPFragment.glsl");
        Window::viewPortChange = !Window::viewPortChange;
    }

    if (Events::justPressed(GLFW_KEY_R)) {
        terrarian->refreshRes();
        fb->refreshShader();
    }

    /* Render to shadow view */
    shadowBuff->bind();
    glcall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    terrarian->render(plr->cam);
    shadowBuff->unbind();

    /* Render to post-processing framebuffer */
    fb->bind();
    glcall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    if (!fb->check()) {
        CONSOLE_LOG("Frame buffer in not complete...");
    }

    skybox->render(plr->cam);
    terrarian->renderShadows(plr->cam, shadowBuff);
    lines->render(plr->cam);
    fb->unbind();

    /* Draw result of postprocessing */
    fb->drawColor();
}

/* Skybox Handler */
SkyboxHandler::SkyboxHandler() {
    current = 0;
    skyboxes[0] = new Skybox("assets/Skybox4.png");
}

void SkyboxHandler::terminate() {
    for (unsigned int i = 0; i < 5; i++) {
        skyboxes[i]->texture->deleteTex();
        delete skyboxes[i];
    }
}

void SkyboxHandler::render(Camera const* cam) {
    skyboxes[current]->render(cam);
}

/* Terrarian handler */
TerrarianHandler::TerrarianHandler() {
    terra = new Terrarian("assets/textureAtlas3.png");
}

void TerrarianHandler::reloadChunks(Camera const* cam) { terra->reload(); }

void TerrarianHandler::terminate() { delete terra; }

void TerrarianHandler::render(Camera const* cam) {
    glcall(glEnable(GL_DEPTH_TEST));
    glcall(glEnable(GL_CULL_FACE));
    terra->render(cam);
    glcall(glDisable(GL_DEPTH_TEST));
    glcall(glDisable(GL_CULL_FACE));
}

void TerrarianHandler::renderShadows(
    Camera const* cam, FrameBuffer* shadowBuff
) {
    glcall(glEnable(GL_DEPTH_TEST));
    glcall(glEnable(GL_CULL_FACE));
    terra->render(cam);
    glcall(glDisable(GL_DEPTH_TEST));
    glcall(glDisable(GL_CULL_FACE));
}

void TerrarianHandler::refreshRes() {
    terra->refreshShader();
    terra->refreshTextures();
}

/* LineBatch handler */
LineBatchHandler::LineBatchHandler() { lineBatch = new LineBatch(4096); }

void LineBatchHandler::terminate() { delete lineBatch; }

void LineBatchHandler::render(Camera const* cam) {
    glcall(glEnable(GL_DEPTH_TEST));
    lineBatch->render(cam);
    glcall(glDisable(GL_DEPTH_TEST));
}
