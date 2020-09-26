#include "GUI.h"
#include "../defines.cpp"
#include <iostream>

#define GUI_VERTEX_SIZE (2 + 2)
#define GUI_VERTEX(I,X,Y,TEX_X,TEX_Y) buffer[I + 0] = X; \
									  buffer[I + 1] = Y; \
									  buffer[I + 2] = TEX_X; \
									  buffer[I + 3] = TEX_Y; \
									  I += GUI_VERTEX_SIZE;
#define RECT(I,X,Y,W,H) GUI_VERTEX(I, X - (W / 2) , Y - (H / 2), 0, 0) \
						GUI_VERTEX(I, X - (W / 2) , Y + (H / 2), 0, 1) \
						GUI_VERTEX(I, X + (W / 2) , Y + (H / 2), 1, 1) \
						GUI_VERTEX(I, X + (W / 2) , Y - (H / 2), 1, 0) \
						GUI_VERTEX(I, X - (W / 2) , Y - (H / 2), 0, 0) \
						GUI_VERTEX(I, X + (W / 2) , Y + (H / 2), 1, 1)

GUI::GUI() {
	int GUIattrs[] = { 2, 2, 0 };
	sprites = new Sprite[64];
	buttons = new Button[64];
	objectsButtons = 0;
	objectsSprites = 0;
	index = 0;
	buffer = new float[(64 + 64) * 6 * GUI_VERTEX_SIZE];
	mesh = new Mesh(buffer, 0, GUIattrs);
}
void GUI::addButton(float posX, float posY, float width, float height, const Texture* defTexture, const Texture* hoverTexture, const Texture* clickedTexture) {
	buttons[objectsButtons] = Button(posX, posY, width, height, defTexture, hoverTexture, clickedTexture);

	RECT(index, posX, posY, width, height);

	objectsButtons++;
}
void GUI::addSprite(float posX, float posY, float width, float height, const Texture* texture) {
	sprites[objectsSprites] = Sprite(posX, posY, width, height, texture);

	RECT(index, posX, posY, width, height);

	objectsSprites++;
}
void GUI::render() {
	if (index == 0)
		return;
	glcall(glDisable(GL_CULL_FACE));
	glcall(glDisable(GL_DEPTH_TEST));
	mesh->reload(buffer, index / GUI_VERTEX_SIZE);
	mesh->draw(GL_TRIANGLES);
	glcall(glEnable(GL_DEPTH_TEST));
	glcall(glEnable(GL_CULL_FACE));
}
int GUI::getObjects() { return objectsButtons + objectsSprites; }