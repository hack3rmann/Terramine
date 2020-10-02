#pragma once

#include "GUI.h"

class GUIHandler {
	GUI* GUIs[8];
	int current;
public:
	GUIHandler();
	void render();
};