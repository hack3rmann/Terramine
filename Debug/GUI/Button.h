#pragma once
#ifndef BUTTON_H_
#define BUTTON_H_

#include "../Graphics/Texture.h"
#include "GUIObject.h"

enum States {
	Default, onHover, onClick
};

class Button : public GUIObject {
	Texture* textures;
	States state;
public:
	Button();
	Button(float posX, float posY, float width, float height, const Texture* defTexture, const Texture* hoverTexture, const Texture* clickedTexture);
};

#endif // !BUTTON_H_
