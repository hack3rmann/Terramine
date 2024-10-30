#include "Voxel.h"

#include "BlockStore.h"

// BlockStore Voxel::blocks =
//     BlockStore("assets/blocks.json", "assets/block_textures.json");
BlockStore Voxel::blocks{};

void Voxel::setID(uint8_t id) {
    if (id != 0) {
        *this = blocks[id - 1];
    } else {
        this->id = 0;
    }
}

Voxel::Voxel(
    bool isTransparent, uint8_t id, uint8_t TopTexId, uint8_t BottomTexId,
    uint8_t LeftTexId, uint8_t RightTexId, uint8_t FrontTexId, uint8_t BackTexId
)
: isTransparent(isTransparent)
, id(id)
, TopTexId(TopTexId)
, BottomTexId(BottomTexId)
, LeftTexId(LeftTexId)
, RightTexId(RightTexId)
, FrontTexId(FrontTexId)
, BackTexId(BackTexId) {}

Voxel::Voxel() {}

Voxel::Voxel(Block const& block) {
    isTransparent = block.isTransparent;
    BottomTexId = block.BottomTexId;
    RightTexId = block.RightTexId;
    FrontTexId = block.FrontTexId;
    LeftTexId = block.LeftTexId;
    BackTexId = block.BackTexId;
    TopTexId = block.TopTexId;
    name = block.name;
    id = block.id;
}
