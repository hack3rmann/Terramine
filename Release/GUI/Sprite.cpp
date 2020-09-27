#include "Sprite.h"

Sprite::Sprite() { }
Sprite::Sprite(float posX, float posY, float width, float height, const Texture* texture) : GUIObject(posX, posY, width, height) {
	this->texture = *new Texture(*texture);
}