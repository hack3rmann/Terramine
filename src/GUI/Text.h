#pragma once

#include <string>
#include <glm/glm.hpp>

#include "../graphics.hpp"
#include "../types.hpp"
#include "../cmp_bmfont.hpp"

struct Charset;

class Text {
    std::string text;
    static Charset chars;
    static tmine::Texture fontTex;
    static tmine::ShaderProgram shader;

    struct Vertex {
        glm::vec2 pos;
        glm::vec2 uv;
        glm::vec3 color;

        static auto constexpr ATTRIBUTE_SIZES =
            std::array<tmine::usize, 3>{2, 2, 3};
    };

    tmine::Mesh<Vertex> mesh;
    glm::vec2 position;
    glm::mat4 model;

    float cursorPointer;
    float fontSize;

public:
    static void init();
    void render(tmine::f32 aspect_ratio);
    void reload();
    static auto get_proj(tmine::f32 aspect_ratio) -> glm::mat4;
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
