#include "Chunk.h"
#include "voxel.h"
#include <math.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include "../defines.cpp"

Chunk::Chunk(int xpos, int ypos, int zpos) : x(xpos), y(ypos), z(zpos) {
	voxels = new voxel[CHUNK_VOL];
	for (int z = 0; z < CHUNK_D; z++) {
		for (int x = 0; x < CHUNK_W; x++) {
			int real_x = x + this->x * CHUNK_W;
			int real_z = z + this->z * CHUNK_D;

			/* Fractal perlin noise */
			float height = glm::perlin(glm::vec3(real_x * 0.0026125f, real_z * 0.0026125f, 0.0f));
			height += glm::perlin(glm::vec3(real_x * 0.006125f, real_z * 0.006125f, 0.0f)) * 0.5f;
			height += glm::perlin(glm::vec3(real_x * 0.018125f, real_z * 0.018125f, 0.0f)) * 0.25f;
			height += glm::perlin(glm::vec3(real_x * 0.038125f, real_z * 0.038125f, 0.0f)) * 0.175f;

			for (int y = 0; y < CHUNK_H; y++) {
				int real_y = y + this->y * CHUNK_H;
				int id = (real_y <= (height) * 30 + 40);
				if (real_y <= (height) * 30 + 39)
					id = 2;
				voxels[(y * CHUNK_D + z) * CHUNK_W + x].setID(id);
			}
		}
	}
}
Chunk::~Chunk() {
	delete[] voxels;
}