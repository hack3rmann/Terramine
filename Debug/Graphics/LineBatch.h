#pragma once

#include "../Camera.h"
#include "Shader.h"

class Mesh;

class LineBatch {
	Mesh* mesh;
	float* buffer;
	unsigned __int64 index;
	unsigned __int64 capacity;
public:
	Shader* shader;

	LineBatch(unsigned __int64 capacity);
	~LineBatch();

	void line(float x1, float y1, float z1, float x2, float y2, float z2, float r, float g, float b, float a);
	void box(float x, float y, float z, float w, float h, float d, float r, float g, float b, float a);
	void render(const Camera* cam);
};
