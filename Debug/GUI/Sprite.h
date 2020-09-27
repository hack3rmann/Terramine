#pragma once
#ifndef SPRITE_H_
#define SPRITE_H_

#include "../Graphics/Texture.h"
#include "GUIObject.h"

class Sprite : public GUIObject {
public:
	Texture texture;
	Sprite();
	Sprite(float posX, float posY, float width, float height, const Texture* texture);
};

#endif // !SPRITE_H_
