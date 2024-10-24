#pragma once

#include <fstream>
#include <glm/glm.hpp>
#include <sstream>
#include <string>

#include "../Graphics/Shader.h"
#include "../Graphics/Texture.h"
#include "../Mesh.h"
#include "../cmp_bmfont.hpp"

namespace FUCK {
    inline int min(int a, int b) { return a > b ? a : b; }
}  // namespace FUCK

struct Charset;

class Text {
    std::string text;
    static Charset chars;
    static Texture* fontTex;
    static Shader* shader;

    float* buffer;
    Mesh* mesh;
    int vertices;
    glm::vec2 position;
    glm::mat4 proj;
    glm::mat4 model;

    float cursorPointer;
    float fontSize;

public:
    static void init();
    void render();
    void reload();
    Text(std::string text, glm::vec2 position, float fontSize);
};

struct CharDescriptor {
    /* Clean 16 bytes */
    unsigned short x, y;
    unsigned short Width, Height;
    float XOffset, YOffset;
    float XAdvance;
    unsigned short Page;

    CharDescriptor()
        : x(0)
        , y(0)
        , Width(0)
        , Height(0)
        , XOffset(0)
        , YOffset(0)
        , XAdvance(0)
        , Page(0) {}
};

struct Charset {
    unsigned short LineHeight;
    unsigned short Base;
    unsigned short Width, Height;
    unsigned short Pages;
    CharDescriptor Chars[256];
};

bool ParseFont(Charset& CharsetDesc);
