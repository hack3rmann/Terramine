#pragma once

#include "GUI.h"
#include "../Graphics/Shader.h"
#include <list>

enum GUIstate {
	nothing,
	startMenu,
	pauseMenu,
	settings
};

class GUIHandler {
	GUI** GUIs;
	Shader* shader;

	Texture* bg;
	Texture* bDef;
	Texture* bHover;
	Texture* bClicked;
	Texture* darker;
public:
	GUIstate current;
	GUIHandler(GUIstate current);
	~GUIHandler();
	void render();
};