#include "Chunks.h"
#include "Chunk.h"
#include "Voxel.h"
#include "../defines.cpp"
#include <iostream>

Chunks::Chunks(int w, int h, int d) : w(w), h(h), d(d) {
	volume = w * h * d;
	chunks = new Chunk*[volume];

	int index = 0;
	int percentage = 0;
	for (int y = 0; y < h; y++) {
		for (int z = 0; z < d; z++) {
			for (int x = 0; x < w; x++, index++) {
				Chunk* chunk = new Chunk(x, y, z);
				chunks[index] = chunk;
			}
		}
		if (percentage != ((float)y / (float)(h - 1)) * 100.f || y == 0) {
			CONSOLE_LOG("Generating terrarian...\t");
			percentage = ((float)y / (float)(h - 1)) * 100.f;
			CONSOLE_LOG(percentage)CONSOLE_LOG("%   \r");
		}
		if (y == h - 1) {
			CONSOLE_LOG("\n");
		}
	}
}
Chunks::~Chunks() {
	for (int i = 0; i < volume; i++) {
		delete chunks[i];
	}
	delete[] chunks;
}
voxel* Chunks::get(int x, int y, int z) const {
	int cx = x / CHUNK_W;
	int cy = y / CHUNK_H;
	int cz = z / CHUNK_D;
	if (x < 0) cx--;
	if (y < 0) cy--;
	if (z < 0) cz--;
	if (cx < 0 || cy < 0 || cz < 0 || cx >= w || cy >= h || cz >= d)
		return nullptr;
	//Chunk* chunk = ;
	int lx = x - cx * CHUNK_W;
	int ly = y - cy * CHUNK_H;
	int lz = z - cz * CHUNK_D;
	return &chunks[(cy * d + cz) * w + cx]->voxels[(ly * CHUNK_D + lz) * CHUNK_W + lx];
}
Chunk* Chunks::getChunk(int x, int y, int z) const {
	if (x < 0 || y < 0 || z < 0 || x >= w || y >= h || z >= d)
		return nullptr;
	return chunks[(y * d + z) * w + x];
}
void Chunks::set(int x, int y, int z, int id) {
	int cx = x / CHUNK_W;
	int cy = y / CHUNK_H;
	int cz = z / CHUNK_D;
	if (x < 0) cx--;
	if (y < 0) cy--;
	if (z < 0) cz--;
	if (cx < 0 || cy < 0 || cz < 0 || cx >= w || cy >= h || cz >= d)
		return;
	Chunk* chunk = chunks[(cy * d + cz) * w + cx];
	int lx = x - cx * CHUNK_W;
	int ly = y - cy * CHUNK_H;
	int lz = z - cz * CHUNK_D;
	chunk->voxels[(ly * CHUNK_D + lz) * CHUNK_W + lx].setID(id);
	chunk->modified = true;

	if (lx == 0 && (chunk = getChunk(cx - 1, cy, cz))) chunk->modified = true;
	if (ly == 0 && (chunk = getChunk(cx, cy - 1, cz))) chunk->modified = true;
	if (lz == 0 && (chunk = getChunk(cx, cy, cz - 1))) chunk->modified = true;

	if (lx == CHUNK_W - 1 && (chunk = getChunk(cx + 1, cy, cz))) chunk->modified = true;
	if (ly == CHUNK_H - 1 && (chunk = getChunk(cx, cy + 1, cz))) chunk->modified = true;
	if (lz == CHUNK_D - 1 && (chunk = getChunk(cx, cy, cz + 1))) chunk->modified = true;
}
voxel* Chunks::rayCast(vec3 a, vec3 dir, float maxDist, vec3& end, vec3& norm, vec3& iend) const {
	float px = a.x;
	float py = a.y;
	float pz = a.z;

	float dx = dir.x;
	float dy = dir.y;
	float dz = dir.z;

	float t = 0.0f;
	int ix = floor(px);
	int iy = floor(py);
	int iz = floor(pz);

	float stepx = (dx > 0.0f) ? 1.0f : -1.0f;
	float stepy = (dy > 0.0f) ? 1.0f : -1.0f;
	float stepz = (dz > 0.0f) ? 1.0f : -1.0f;

	float infinity = std::numeric_limits<float>::infinity();

	float txDelta = (dx == 0.0f) ? infinity : abs(1.0f / dx);
	float tyDelta = (dy == 0.0f) ? infinity : abs(1.0f / dy);
	float tzDelta = (dz == 0.0f) ? infinity : abs(1.0f / dz);

	float xdist = (stepx > 0) ? (ix + 1 - px) : (px - ix);
	float ydist = (stepy > 0) ? (iy + 1 - py) : (py - iy);
	float zdist = (stepz > 0) ? (iz + 1 - pz) : (pz - iz);

	float txMax = (txDelta < infinity) ? txDelta * xdist : infinity;
	float tyMax = (tyDelta < infinity) ? tyDelta * ydist : infinity;
	float tzMax = (tzDelta < infinity) ? tzDelta * zdist : infinity;

	int steppedIndex = -1;

	while (t <= maxDist) {
		voxel* voxel = get(ix, iy, iz);
		if (voxel == nullptr || voxel->id) {
			end.x = px + t * dx;
			end.y = py + t * dy;
			end.z = pz + t * dz;

			iend.x = ix;
			iend.y = iy;
			iend.z = iz;

			norm.x = norm.y = norm.z = 0.0f;
			if (steppedIndex == 0) norm.x = -stepx;
			if (steppedIndex == 1) norm.y = -stepy;
			if (steppedIndex == 2) norm.z = -stepz;
			return voxel;
		}
		if (txMax < tyMax) {
			if (txMax < tzMax) {
				ix += stepx;
				t = txMax;
				txMax += txDelta;
				steppedIndex = 0;
			} else {
				iz += stepz;
				t = tzMax;
				tzMax += tzDelta;
				steppedIndex = 2;
			}
		} else {
			if (tyMax < tzMax) {
				iy += stepy;
				t = tyMax;
				tyMax += tyDelta;
				steppedIndex = 1;
			} else {
				iz += stepz;
				t = tzMax;
				tzMax += tzDelta;
				steppedIndex = 2;
			}
		}
	}
	iend.x = ix;
	iend.y = iy;
	iend.z = iz;

	end.x = px + t * dx;
	end.y = py + t * dy;
	end.z = pz + t * dz;
	norm.x = norm.y = norm.z = 0.0f;
	return nullptr;
}
voxel* Chunks::rayCast(vec3 a, vec3 dir, float maxDist) const {
	float px = a.x;
	float py = a.y;
	float pz = a.z;

	float dx = dir.x;
	float dy = dir.y;
	float dz = dir.z;

	float t = 0.0f;
	int ix = floor(px);
	int iy = floor(py);
	int iz = floor(pz);

	float stepx = (dx > 0.0f) ? 1.0f : -1.0f;
	float stepy = (dy > 0.0f) ? 1.0f : -1.0f;
	float stepz = (dz > 0.0f) ? 1.0f : -1.0f;

	float infinity = std::numeric_limits<float>::infinity();

	float txDelta = (dx == 0.0f) ? infinity : abs(1.0f / dx);
	float tyDelta = (dy == 0.0f) ? infinity : abs(1.0f / dy);
	float tzDelta = (dz == 0.0f) ? infinity : abs(1.0f / dz);

	float xdist = (stepx > 0) ? (ix + 1 - px) : (px - ix);
	float ydist = (stepy > 0) ? (iy + 1 - py) : (py - iy);
	float zdist = (stepz > 0) ? (iz + 1 - pz) : (pz - iz);

	float txMax = (txDelta < infinity) ? txDelta * xdist : infinity;
	float tyMax = (tyDelta < infinity) ? tyDelta * ydist : infinity;
	float tzMax = (tzDelta < infinity) ? tzDelta * zdist : infinity;

	int steppedIndex = -1;

	while (t <= maxDist) {
		voxel* voxel = get(ix, iy, iz);
		if (voxel == nullptr || voxel->id) {
			//end.x = px + t * dx;
			//end.y = py + t * dy;
			//end.z = pz + t * dz;

			//iend.x = ix;
			//iend.y = iy;
			//iend.z = iz;

			//norm.x = norm.y = norm.z = 0.0f;
			//if (steppedIndex == 0) norm.x = -stepx;
			//if (steppedIndex == 1) norm.y = -stepy;
			//if (steppedIndex == 2) norm.z = -stepz;
			return voxel;
		}
		if (txMax < tyMax) {
			if (txMax < tzMax) {
				ix += stepx;
				t = txMax;
				txMax += txDelta;
				steppedIndex = 0;
			} else {
				iz += stepz;
				t = tzMax;
				tzMax += tzDelta;
				steppedIndex = 2;
			}
		} else {
			if (tyMax < tzMax) {
				iy += stepy;
				t = tyMax;
				tyMax += tyDelta;
				steppedIndex = 1;
			} else {
				iz += stepz;
				t = tzMax;
				tzMax += tzDelta;
				steppedIndex = 2;
			}
		}
	}
	//iend.x = ix;
	//iend.y = iy;
	//iend.z = iz;
	//
	//end.x = px + t * dx;
	//end.y = py + t * dy;
	//end.z = pz + t * dz;
	//norm.x = norm.y = norm.z = 0.0f;
	return nullptr;
}
voxel* Chunks::rayCast(vec3 a, vec3 dir, float maxDist, vec3& norm) const {
	float px = a.x;
	float py = a.y;
	float pz = a.z;

	float dx = dir.x;
	float dy = dir.y;
	float dz = dir.z;

	float t = 0.0f;
	int ix = floor(px);
	int iy = floor(py);
	int iz = floor(pz);

	float stepx = (dx > 0.0f) ? 1.0f : -1.0f;
	float stepy = (dy > 0.0f) ? 1.0f : -1.0f;
	float stepz = (dz > 0.0f) ? 1.0f : -1.0f;

	float infinity = std::numeric_limits<float>::infinity();

	float txDelta = (dx == 0.0f) ? infinity : abs(1.0f / dx);
	float tyDelta = (dy == 0.0f) ? infinity : abs(1.0f / dy);
	float tzDelta = (dz == 0.0f) ? infinity : abs(1.0f / dz);

	float xdist = (stepx > 0) ? (ix + 1 - px) : (px - ix);
	float ydist = (stepy > 0) ? (iy + 1 - py) : (py - iy);
	float zdist = (stepz > 0) ? (iz + 1 - pz) : (pz - iz);

	float txMax = (txDelta < infinity) ? txDelta * xdist : infinity;
	float tyMax = (tyDelta < infinity) ? tyDelta * ydist : infinity;
	float tzMax = (tzDelta < infinity) ? tzDelta * zdist : infinity;

	int steppedIndex = -1;

	while (t <= maxDist) {
		voxel* voxel = get(ix, iy, iz);
		if (voxel == nullptr || voxel->id) {
			norm.x = norm.y = norm.z = 0.0f;
			if (steppedIndex == 0) norm.x = -stepx;
			if (steppedIndex == 1) norm.y = -stepy;
			if (steppedIndex == 2) norm.z = -stepz;
			return voxel;
		}
		if (txMax < tyMax) {
			if (txMax < tzMax) {
				ix += stepx;
				t = txMax;
				txMax += txDelta;
				steppedIndex = 0;
			} else {
				iz += stepz;
				t = tzMax;
				tzMax += tzDelta;
				steppedIndex = 2;
			}
		} else {
			if (tyMax < tzMax) {
				iy += stepy;
				t = tyMax;
				tyMax += tyDelta;
				steppedIndex = 1;
			} else {
				iz += stepz;
				t = tzMax;
				tzMax += tzDelta;
				steppedIndex = 2;
			}
		}
	}
	norm.x = norm.y = norm.z = 0.0f;
	return nullptr;
}