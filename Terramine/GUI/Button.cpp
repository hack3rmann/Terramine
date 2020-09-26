#include "Button.h"

Button::Button() {
	state = Default;
	textures = new Texture[3];
}
Button::Button(float posX, float posY, float width, float height, const Texture* defTexture, const Texture* hoverTexture, const Texture* clickedTexture)
	: GUIObject(posX, posY, width, height) {
	textures[0] = *defTexture;
	textures[1] = *hoverTexture;
	textures[2] = *clickedTexture;
	state = Default;
}