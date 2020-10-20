#include "GUI.h"
#include "../defines.cpp"
#include <iostream>

GUI::GUI() {
	objectsButtons = 0;
	objectsSprites = 0;
	size = 0;
}
void GUI::addButton(float posX, float posY, float width, float height,
					const Texture* defTexture, const Texture* hoverTexture, const Texture* clickedTexture,
					std::string text,
					std::function<void()> function) {
	buttons[objectsButtons] = new Button(posX, posY, width, height, defTexture, hoverTexture, clickedTexture, text, function);

	objectsButtons++;
	size++;
}
void GUI::addSprite(float posX, float posY, float width, float height, const Texture* texture) {
	sprites[objectsSprites] = new Sprite(posX, posY, width, height, texture);

	objectsSprites++;
	size++;
}
void GUI::render() {
	/* Render all sorites */
	for (int i = 0; i < objectsSprites; i++)
		sprites[i]->render();

	/* Render all buttons */
	for (int i = 0; i < objectsButtons; i++) {
		buttons[i]->refreshState();
		buttons[i]->render();
	}
}
int GUI::getObjects() { return size; }
void GUI::cleanUp() {
	for (int i = 0; i < objectsButtons; i++) {
		buttons[i]->cleanUp();
		delete buttons[i];
	}
	for (int i = 0; i < objectsSprites; i++) {
		sprites[i]->cleanUp();
		delete sprites[i];
	}
}