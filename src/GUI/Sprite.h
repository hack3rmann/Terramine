#pragma once
#ifndef SPRITE_H_
#define SPRITE_H_

#include "../Graphics/Texture.h"
#include "../Graphics/Shader.h"
#include "../Mesh.h"
#include "GUIObject.h"
#include <glm/glm.hpp>

class Sprite : public GUIObject {
public:
	Mesh* mesh;
	float* buffer;
	Texture* texture;

	Shader* shader;

	glm::mat4 proj;
	glm::mat4 model;

	float x, y;

	Sprite();
	Sprite(float posX, float posY, float width, float height, const Texture* texture);
	void render();
	void cleanUp();
};

#endif // !SPRITE_H_
