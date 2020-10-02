#include "SceneHandler.h"
#include "../EventHandler.h"

/* Scene Handler */ 
SceneHandler::SceneHandler() {
	plr = new Player(glfwGetTime(), -30.0f, vec3(0.0f));
	terrarian = new TerrarianHandler();
	lines = new LineBatchHandler();
	skybox = new SkyboxHandler();
	//shadowFB = new ShadowFB();
	fb = new FrameBuffer();
}
void SceneHandler::terminate() {
	terrarian->terminate();
	lines->terminate();
	skybox->terminate();
	//shadowFB->terminate();
	fb->terminate();
	delete plr;
	delete fb;
	delete terrarian;
	delete lines;
	delete skybox;
	//delete shadowFB;
}
void SceneHandler::updateAll() {
	plr->update(terrarian->terra->chunks, lines->lineBatch);
	terrarian->reloadChunks();
}
void SceneHandler::updateChunks() {
	terrarian->reloadChunks();
}
void SceneHandler::updatePlayer() {
	plr->update(terrarian->terra->chunks, lines->lineBatch);
}
void SceneHandler::render() {
	if (Window::viewPortChange) {
		fb->reload();
		Window::viewPortChange = !Window::viewPortChange;
	}

	if (Events::justPressed(GLFW_KEY_R)) {
		terrarian->refreshRes();
		fb->refreshShader();
	}

	fb->bind();
		glcall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		if (!fb->check()) CONSOLE_LOG("Frame buffer in not complete...");
		skybox->render(plr->cam);
		terrarian->render(plr->cam);
		lines->render(plr->cam);
	fb->unbind();

	fb->draw();
}

/* Skybox Handler */
SkyboxHandler::SkyboxHandler() {
	current = 0;
	skyboxes[0] = new Skybox("src/Skybox4.png");
}
void SkyboxHandler::terminate() {
	for (unsigned int i = 0; i < 5; i++) {
		skyboxes[i]->texture->deleteTex();
		delete skyboxes[i];
	}
}
void SkyboxHandler::render(const Camera* cam) {
	skyboxes[current]->render(cam);
}

/* Terrarian handler */
TerrarianHandler::TerrarianHandler() {
	terra = new Terrarian("src/textureAtlas3.png");
}
void TerrarianHandler::reloadChunks() {
	terra->reload();
}
void TerrarianHandler::terminate() {
	delete terra;
}
void TerrarianHandler::render(const Camera* cam) {
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
LineBatchHandler::LineBatchHandler() {
	lineBatch = new LineBatch(4096);
}
void LineBatchHandler::terminate() {
	delete lineBatch;
}
void LineBatchHandler::render(const Camera* cam) {
	glcall(glEnable(GL_DEPTH_TEST));
	lineBatch->render(cam);
	glcall(glDisable(GL_DEPTH_TEST));
}