#pragma once
#ifndef BUTTON_H_
#define BUTTON_H_

#include "../Graphics/Texture.h"
#include "../Mesh.h"
#include "Text.h"
#include "GUIObject.h"
#include <string>
#include <functional>
#include <glm/glm.hpp>

enum States {
	Default, onHover, onClick
};

class Button : public GUIObject {
	Texture* textures[3];
	std::function<void()> function;
	Mesh* mesh;
	float* buffer;
	States state;
	Shader* shader;
	float x, y, w, h;
	Text* text;

	glm::mat4 proj;
	glm::mat4 model;
public:
	Button();
	Button(float posX, float posY, float width, float height,
		   const Texture* defTexture, const Texture* hoverTexture, const Texture* clickedTexture,
		   std::string text,
		   std::function<void()> function);
	void render();
	void refreshState();
	void cleanUp();
};

#endif // !BUTTON_H_
