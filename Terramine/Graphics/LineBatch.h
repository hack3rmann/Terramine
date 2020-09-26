#pragma once
#ifndef LINE_BATCH_H_

class Mesh;

class LineBatch {
	Mesh* mesh;
	float* buffer;
	size_t index;
	size_t capacity;
public:
	LineBatch(size_t capacity);
	~LineBatch();

	void line(float x1, float y1, float z1, float x2, float y2, float z2, float r, float g, float b, float a);
	void box(float x, float y, float z, float w, float h, float d, float r, float g, float b, float a);
	void render();
};

#define LINE_BATCH_H_
#endif /* !LINE_BATCH_H_ */