#include "BlockStore.h"

#include <rapidjson/document.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "../defines.cpp"

using namespace std;
using namespace rapidjson;

BlockStore::BlockStore(
    std::string const& BlockFile, std::string const& BlockTextureFile
) {
    /* Init reading */
    string jsonBlock;
    string jsonTexture;
    ifstream fileBlock;
    ifstream fileTexture;
    stringstream fileBlockStream;
    stringstream fileTextureStream;

    /* Opening */
    fileBlock.open(BlockFile);
    fileTexture.open(BlockTextureFile);

    /* If not open, throw console log */
    if (!fileBlock.is_open() || !fileTexture.is_open()) {
        CONSOLE_LOG(
            "Can not load file " + BlockFile + " or " + BlockTextureFile + "!\n"
        );
    }
    /* If load, parse */
    else
    {
        /* Read */
        fileBlockStream << fileBlock.rdbuf();
        fileTextureStream << fileTexture.rdbuf();

        /* Close used files */
        fileBlock.close();
        fileTexture.close();

        /* Creating documents and parse */
        jsonBlock = fileBlockStream.str();
        jsonTexture = fileTextureStream.str();
        BlockDOC.Parse(jsonBlock.c_str());
        TextureDOC.Parse(jsonTexture.c_str());

        /* It's wrong, if documents are not arrays */
        if (!BlockDOC.IsArray()) {
            CONSOLE_LOG(
                "Document \"Voxels\\" + BlockFile + "\" is not array!\n"
            );
        }
        if (!TextureDOC.IsArray()) {
            CONSOLE_LOG(
                "Document \"Voxels\\" + BlockTextureFile + "\" is not array!\n"
            );
        }

        /* Creating arrays */
        textures = new BlockTexture[TextureDOC.Size()];
        blockTypes = new Block[BlockDOC.Size()];

        /* Initing textures */
        for (uint32_t i = 0; i < TextureDOC.Size(); i++) {
            textures[i].name =
                new std::string(TextureDOC[i]["Name"].GetString());
            textures[i].id = TextureDOC[i]["id"].GetInt();
        }

        for (uint32_t i = 0; i < BlockDOC.Size(); i++) {
            blockTypes[i].name =
                new std::string(BlockDOC[i]["Name"].GetString());
            blockTypes[i].id = BlockDOC[i]["id"].GetInt();

            if (BlockDOC[i].HasMember("isTransparent")) {
                blockTypes[i].isTransparent =
                    BlockDOC[i]["isTransparent"].GetBool();
            } else {
                blockTypes[i].isTransparent = false;
            }

            blockTypes[i].Variation = BlockDOC[i]["Variation"].GetInt();

            /* Have "AllSides" */
            if (BlockDOC[i].HasMember("AllSides")) {
                uint8_t id;
                for (uint32_t j = 0; j < TextureDOC.Size(); j++) {
                    if (*textures[j].name ==
                        BlockDOC[i]["AllSides"].GetString())
                    {
                        id = textures[j].id;
                        break;
                    }
                }
                blockTypes[i].TopTexId = blockTypes[i].BottomTexId =
                    blockTypes[i].LeftTexId = blockTypes[i].RightTexId =
                        blockTypes[i].FrontTexId = blockTypes[i].BackTexId = id;

                /* Have "Sides" */
            } else if (BlockDOC[i].HasMember("Sides")) {
                uint8_t idSides, idTop, idBottom;
                for (uint32_t j = 0; j < TextureDOC.Size(); j++) {
                    if (*textures[j].name == BlockDOC[i]["Sides"].GetString()) {
                        idSides = textures[j].id;
                        break;
                    }
                }
                for (uint32_t j = 0; j < TextureDOC.Size(); j++) {
                    if (*textures[j].name == BlockDOC[i]["Top"].GetString()) {
                        idTop = textures[j].id;
                        break;
                    }
                }
                for (uint32_t j = 0; j < TextureDOC.Size(); j++) {
                    if (*textures[j].name == BlockDOC[i]["Bottom"].GetString())
                    {
                        idBottom = textures[j].id;
                        break;
                    }
                }
                blockTypes[i].TopTexId = idTop;
                blockTypes[i].BottomTexId = idBottom;
                blockTypes[i].LeftTexId = blockTypes[i].RightTexId =
                    blockTypes[i].FrontTexId = blockTypes[i].BackTexId =
                        idSides;

                /* All sides unique */
            } else {
                uint8_t idTop, idBottom, idLeft, idRight, idFront, idBack;
                for (uint32_t j = 0; j < TextureDOC.Size(); j++) {
                    if (*textures[j].name == BlockDOC[i]["Top"].GetString()) {
                        idTop = textures[j].id;
                        break;
                    }
                }
                for (uint32_t j = 0; j < TextureDOC.Size(); j++) {
                    if (*textures[j].name == BlockDOC[i]["Bottom"].GetString())
                    {
                        idBottom = textures[j].id;
                        break;
                    }
                }
                for (uint32_t j = 0; j < TextureDOC.Size(); j++) {
                    if (*textures[j].name == BlockDOC[i]["Left"].GetString()) {
                        idLeft = textures[j].id;
                        break;
                    }
                }
                for (uint32_t j = 0; j < TextureDOC.Size(); j++) {
                    if (*textures[j].name == BlockDOC[i]["Right"].GetString()) {
                        idRight = textures[j].id;
                        break;
                    }
                }
                for (uint32_t j = 0; j < TextureDOC.Size(); j++) {
                    if (*textures[j].name == BlockDOC[i]["Front"].GetString()) {
                        idFront = textures[j].id;
                        break;
                    }
                }
                for (uint32_t j = 0; j < TextureDOC.Size(); j++) {
                    if (*textures[j].name == BlockDOC[i]["Back"].GetString()) {
                        idBack = textures[j].id;
                        break;
                    }
                }
                blockTypes[i].TopTexId = idTop;
                blockTypes[i].BottomTexId = idBottom;
                blockTypes[i].LeftTexId = idLeft;
                blockTypes[i].RightTexId = idRight;
                blockTypes[i].FrontTexId = idFront;
                blockTypes[i].BackTexId = idBack;
            }
        }
    }
}

Block BlockStore::operator[](int id) { return blockTypes[id]; }
