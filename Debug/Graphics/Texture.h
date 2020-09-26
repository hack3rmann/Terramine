#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include "../Loaders/pngLoader.h"

class Texture {
public:
	unsigned int id;
	int width, height;
	Texture(unsigned int id, int width, int height);
	~Texture();

	void bind();
};

#endif // !TEXTURE_H
