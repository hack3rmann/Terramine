#include "MasterHandler.h"
#include "../defines.cpp"

SceneHandler* MasterHandler::sceneHandler;
GUIHandler* MasterHandler::gui;

void MasterHandler::init() {
	sceneHandler = new SceneHandler();
	gui = new GUIHandler();
}
void MasterHandler::render() {
	sceneHandler->render();
	gui->render();
}
void MasterHandler::updateAll() {
	sceneHandler->updateAll();
}