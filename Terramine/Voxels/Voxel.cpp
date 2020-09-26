#include "voxel.h"
#include "BlockStore.h"

BlockStore voxel::blocks = BlockStore("Voxels/Blocks.json", "Voxels/BlockTextures.json");

void voxel::setID(uint8_t id) {
	if (id != 0)
		*this = blocks[id - 1];
	else
		this->id = 0;
}
voxel::voxel(bool isTransparent, uint8_t id, uint8_t TopTexId, uint8_t BottomTexId,
			 uint8_t LeftTexId, uint8_t RightTexId, uint8_t FrontTexId, uint8_t BackTexId)
			: isTransparent(isTransparent), id(id), TopTexId(TopTexId), BottomTexId(BottomTexId)
			, LeftTexId(LeftTexId), RightTexId(RightTexId), FrontTexId(FrontTexId), BackTexId(BackTexId) { }
voxel::voxel() {

}
voxel::voxel(const Block& block) {
	isTransparent = block.isTransparent;
	id = block.id;
	name = block.name;
	TopTexId = block.TopTexId;
	BottomTexId = block.BottomTexId;
	LeftTexId = block.LeftTexId;
	RightTexId = block.RightTexId;
	FrontTexId = block.FrontTexId;
	BackTexId = block.BackTexId;
}