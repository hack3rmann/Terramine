#pragma once
#ifndef GUI_H_
#define GUI_H_

#include "Button.h"
#include "Sprite.h"
#include "../Mesh.h"
#include "GUIObject.h"

class GUI {
	friend class GUIHandler;

	Mesh* mesh;
	float* buffer;
	int objectsButtons;
	int objectsSprites;
	int index;
	GUIObject* buttons;
public:
	Sprite* sprites;
	GUI();
	void addButton(float posX, float posY, float width, float height, const Texture* defTexture, const Texture* hoverTexture, const Texture* clickedTexture);
	void addSprite(float posX, float posY, float width, float height, const Texture* texture);
	int getObjects();
	void render();
};

#endif // !GUI_H_
