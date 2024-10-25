#pragma once

#include <rapidjson/document.h>
#include <string>

#include "Voxel.h"

struct Block;
struct BlockTexture;

class BlockStore {
    rapidjson::Document BlockDOC;
    rapidjson::Document TextureDOC;

public:
    Block* blockTypes;
    BlockTexture* textures;

    BlockStore(
        std::string const& BlockFile, std::string const& BlockTextureFile
    );
    /*voxel getBlockByID(int id, int variation) {
            voxel res = blockTypes[id];
    }*/
    Block operator[](int id);
};

struct Block {
    bool isTransparent;
    std::string* name;
    uint8_t Variation;  // if == 0 then not exist
    uint8_t id;
    uint8_t TopTexId;
    uint8_t BottomTexId;
    uint8_t LeftTexId;
    uint8_t RightTexId;
    uint8_t FrontTexId;
    uint8_t BackTexId;
};

struct BlockTexture {
    std::string* name;
    uint8_t id;
};
