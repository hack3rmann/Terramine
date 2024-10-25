#include "Text.h"

#include <fstream>
#include <sstream>

#include <GLFW/glfw3.h>

#include "../EventHandler.h"
#include "../Window.h"
#include "../types.hpp"

#define TEXT_VERTEX_SIZE (2 + 2 + 3) /* XY TS RGB */
#define TEXT_VERTEX(I, X, Y, T, S, R, G, B) \
    buffer[I + 0] = X;                      \
    buffer[I + 1] = Y;                      \
    buffer[I + 2] = T;                      \
    buffer[I + 3] = S;                      \
    buffer[I + 4] = R;                      \
    buffer[I + 5] = G;                      \
    buffer[I + 6] = B;                      \
    I += TEXT_VERTEX_SIZE;
#define TEXT_QUAD(I, RX, RY, RW, RH, X, Y, W, H, R, G, B) \
    TEXT_VERTEX(I, RX, RY + RH, X, Y, R, G, B);           \
    TEXT_VERTEX(I, RX, RY, X, Y - H, R, G, B);            \
    TEXT_VERTEX(I, RX + RW, RY, X + W, Y - H, R, G, B);   \
    TEXT_VERTEX(I, RX, RY + RH, X, Y, R, G, B);           \
    TEXT_VERTEX(I, RX + RW, RY + RH, X + W, Y, R, G, B);  \
    TEXT_VERTEX(I, RX + RW, RY, X + W, Y - H, R, G, B);

using namespace tmine;

/* Static fields init */
Texture* Text::fontTex;
Charset Text::chars;
Shader* Text::shader;

void Text::init() {
    ParseFont(chars);
    fontTex = load_texture_NO_MIPMAP_LINEAR("assets/font.png");
    shader = load_shader("textVertex.glsl", "textFragment.glsl");
}

Text::Text(std::string text, glm::vec2 position, float fontSize)
    : text(text) {
    /* init */
    float aspect = (float) Window::width / (float) Window::height;
    this->position = position;
    this->fontSize = fontSize;
    int attrs[] = {2, 2, 3, 0};
    buffer = new float[TEXT_VERTEX_SIZE * 6 * 256];
    mesh = new Mesh(buffer, 0, attrs);
    proj = glm::ortho(-1.0f, 1.0f, -aspect, aspect, 0.0f, 100.0f);

    /* Calculating length */
    float length = 0.0;
    for (unsigned int i = 0; i < text.size(); i++) {
        auto& curr = chars.Chars[(usize) text[i]];
        float sizeW = chars.Width;

        length += (curr.XAdvance) / sizeW * fontSize;
    }

    cursorPointer = -length / 2.0f;
    vertices = 0;

    /* Load all quads to array */
    for (unsigned int i = 0; i < text.size(); i++) {
        auto& curr = chars.Chars[(usize) text[i]];
        float sizeW = chars.Width;
        float sizeH = chars.Height;
        float lh = chars.LineHeight;
        float w = curr.Width;
        float h = curr.Height;
        float rw = curr.Width * fontSize;
        float rh = curr.Height * fontSize;
        float x = curr.x;
        float y = sizeH - curr.y;

        TEXT_QUAD(
            vertices, cursorPointer,
            ((lh - curr.YOffset) - h - lh / 2.0f) / sizeH, rw / sizeW,
            rh / sizeH, x / sizeW, y / sizeH, w / sizeW, h / sizeH, 1.0f, 1.0f,
            1.0f
        );

        cursorPointer += (curr.XAdvance) / sizeW * fontSize;
    }
}

void Text::render() {
    if (Events::justPressed(GLFW_KEY_R)) {
        shader = load_shader("textVertex.glsl", "textFragment.glsl");
        reload();
    }
    shader->use();

    /* Texture */
    fontTex->bind();

    /* Matrix init */
    float aspect = (float) Window::height / (float) Window::width;
    proj = glm::ortho(-1.0f, 1.0f, -aspect, aspect, 0.0f, 100.0f);
    model = glm::translate(
        glm::mat4(1.0f), glm::vec3(position.x, position.y, 0.0f)
    );

    /* Shader uniforms */
    shader->uniformMatrix("modelProj", model * proj);

    /* Draw */
    mesh->reload(buffer, vertices);
    mesh->draw(GL_TRIANGLES);
}

void Text::reload() {
    float aspect = (float) Window::width / (float) Window::height;
    proj = glm::ortho(-1.0f, 1.0f, -aspect, aspect, 0.0f, 100.0f);

    float length = 0.0;
    for (unsigned int i = 0; i < text.size(); i++) {
        auto& curr = chars.Chars[(usize) text[i]];
        float sizeW = chars.Width;

        length += (curr.XAdvance) / sizeW * fontSize;
    }

    cursorPointer = -length / 2.0f;
    vertices = 0;

    for (unsigned int i = 0; i < text.size(); i++) {
        auto& curr = chars.Chars[(usize) text[i]];
        float sizeW = chars.Width;
        float sizeH = chars.Height;
        float lh = chars.LineHeight;
        float w = curr.Width;
        float h = curr.Height;
        float rw = curr.Width * fontSize;
        float rh = curr.Height * fontSize;
        float x = curr.x;
        float y = sizeH - curr.y;

        TEXT_QUAD(
            vertices, cursorPointer,
            ((lh - curr.YOffset) - h - lh / 2.0f) / sizeH, rw / sizeW,
            rh / sizeH, x / sizeW, y / sizeH, w / sizeW, h / sizeH, 1.0f, 1.0f,
            1.0f
        );

        cursorPointer += (curr.XAdvance) / sizeW * fontSize;
    }
}

bool ParseFont(Charset& CharsetDesc) {
    std::ifstream Stream;
    std::string Line;
    std::string Read, Key, Value;
    std::size_t i;
    Stream.open("assets/font.fnt");
    while (!Stream.eof()) {
        std::stringstream LineStream;
        std::getline(Stream, Line);
        LineStream << Line;

        /* Read the line's type */
        LineStream >> Read;
        if (Read == "common") {
            /* This holds common data */
            while (!LineStream.eof()) {
                std::stringstream Converter;
                LineStream >> Read;
                i = Read.find('=');
                Key = Read.substr(0, i);
                Value = Read.substr(i + 1);

                /* Assign the correct value */
                Converter << Value;
                if (Key == "lineHeight") {
                    Converter >> CharsetDesc.LineHeight;
                } else if (Key == "base") {
                    Converter >> CharsetDesc.Base;
                } else if (Key == "scaleW") {
                    Converter >> CharsetDesc.Width;
                } else if (Key == "scaleH") {
                    Converter >> CharsetDesc.Height;
                } else if (Key == "pages") {
                    Converter >> CharsetDesc.Pages;
                }
            }
        } else if (Read == "char") {
            /* This is data for a specific char */
            unsigned short CharID = 0;

            while (!LineStream.eof()) {
                std::stringstream Converter;
                LineStream >> Read;
                i = Read.find('=');
                Key = Read.substr(0, i);
                Value = Read.substr(i + 1);

                /* Asign the correct value */
                Converter << Value;
                if (Key == "id") {
                    Converter >> CharID;
                } else if (Key == "x") {
                    Converter >> CharsetDesc.Chars[CharID].x;
                } else if (Key == "y") {
                    Converter >> CharsetDesc.Chars[CharID].y;
                } else if (Key == "width") {
                    Converter >> CharsetDesc.Chars[CharID].Width;
                } else if (Key == "height") {
                    Converter >> CharsetDesc.Chars[CharID].Height;
                } else if (Key == "xoffset") {
                    Converter >> CharsetDesc.Chars[CharID].XOffset;
                } else if (Key == "yoffset") {
                    Converter >> CharsetDesc.Chars[CharID].YOffset;
                } else if (Key == "xadvance") {
                    Converter >> CharsetDesc.Chars[CharID].XAdvance;
                } else if (Key == "page") {
                    Converter >> CharsetDesc.Chars[CharID].Page;
                }
            }
        }
    }

    return true;
}
