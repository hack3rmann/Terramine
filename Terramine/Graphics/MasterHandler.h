#pragma once

#include "SceneHandler.h"
#include "../GUI/GUIHandler.h"
#include <GL/glew.h>

class MasterHandler {
public:
	static SceneHandler* sceneHandler;
	static GUIHandler* gui;

	static void init();
	static void render();
	static void updateAll();
};