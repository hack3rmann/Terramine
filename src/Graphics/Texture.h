#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include "../defines.cpp"
#include "../Loaders/pngLoader.h"
#include <GL/glew.h>

class Texture {
public:
	GLuint id;
	int width, height;
	int slot;
	static int AviableSlot;
	Texture();
	Texture(GLuint id, int width, int height);
	~Texture();

	void bind();
	void deleteTex();
	static void unbind();
};

#endif // !TEXTURE_H
