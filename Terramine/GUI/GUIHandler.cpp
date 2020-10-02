#include "GUIHandler.h"

GUIHandler::GUIHandler() {
	current = 0;
	for (int i = 0; i < 8; i++)
		GUIs[i] = new GUI();
}
void GUIHandler::render() {
	for (auto gui : GUIs)
		gui->render();
}