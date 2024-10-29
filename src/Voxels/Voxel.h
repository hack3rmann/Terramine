#pragma once

#include <stdint.h>

#include <string>

struct Block;
class BlockStore;

class Voxel {
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
    Voxel();
    Voxel(
        bool isTransparent, uint8_t id, uint8_t TopTexId, uint8_t BottomTexId,
        uint8_t LeftTexId, uint8_t RightTexId, uint8_t FrontTexId,
        uint8_t BackTexId
    );
    Voxel(Block const& block);
};
