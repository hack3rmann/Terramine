#pragma once
#ifndef CHUNKS_H_
#define CHUNKS_H_

#include <stdlib.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

using namespace glm;

class Chunk;
class voxel;

class Chunks {
public:
	Chunk** chunks;
	unsigned long long volume;
	unsigned int w, h, d;
	bool modified = true;

	Chunks(int w, int h, int d);
	~Chunks();

	voxel* get(int x, int y, int z) const;
	Chunk* getChunk(int x, int y, int z) const;
	void set(int x, int y, int z, int id);
	voxel* rayCast(vec3 a, vec3 dir, float maxDist, vec3& end, vec3& norm, vec3& iend) const;
	voxel* rayCast(vec3 a, vec3 dir, float maxDist, vec3& norm) const;
	voxel* rayCast(vec3 a, vec3 dir, float maxDist) const;
};

#endif // !CHUNKS_H_
