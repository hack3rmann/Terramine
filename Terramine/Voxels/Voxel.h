#pragma once
#ifndef VOXEL_H_
#define VOXEL_H_

#include <stdint.h>
#include <string>
struct Block;
class BlockStore;

class voxel {
public:
	static BlockStore blocks;
	bool isTransparent;
	std::string* name;
	uint8_t id;
	uint8_t TopTexId;
	uint8_t BottomTexId;
	uint8_t LeftTexId;
	uint8_t RightTexId;
	uint8_t FrontTexId;
	uint8_t BackTexId;
	void setID(uint8_t id);
	voxel();
	voxel(bool isTransparent, uint8_t id, uint8_t TopTexId, uint8_t BottomTexId,
		  uint8_t LeftTexId, uint8_t RightTexId, uint8_t FrontTexId, uint8_t BackTexId);
	voxel(const Block& block);
};

#endif // !VOXEL_H_
