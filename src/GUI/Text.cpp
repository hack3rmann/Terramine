#include "Text.h"

#include <fstream>
#include <sstream>
#include <fmt/format.h>

#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include "../types.hpp"
#include "../loaders.hpp"
#include "../events.hpp"

#define TEXT_VERTEX_SIZE (2 + 2 + 3) /* XY TS RGB */
#define TEXT_VERTEX(I, X, Y, T, S, R, G, B)                              \
    ({                                                                   \
        auto const next_data =                                           \
            std::array<f32, TEXT_VERTEX_SIZE>{X, Y, T, S, R, G, B};      \
        buffer.insert(buffer.end(), next_data.begin(), next_data.end()); \
    })
#define TEXT_QUAD(I, RX, RY, RW, RH, X, Y, W, H, R, G, B)    \
    ({                                                       \
        TEXT_VERTEX(I, RX, RY + RH, X, Y, R, G, B);          \
        TEXT_VERTEX(I, RX, RY, X, Y - H, R, G, B);           \
        TEXT_VERTEX(I, RX + RW, RY, X + W, Y - H, R, G, B);  \
        TEXT_VERTEX(I, RX, RY + RH, X, Y, R, G, B);          \
        TEXT_VERTEX(I, RX + RW, RY + RH, X + W, Y, R, G, B); \
        TEXT_VERTEX(I, RX + RW, RY, X + W, Y - H, R, G, B);  \
    })

using namespace tmine;

/* Static fields init */
Texture Text::fontTex;
Charset Text::chars;
ShaderProgram Text::shader;

void Text::init() {
    ParseFont(chars);
    fontTex = Texture::from_image(
        load_png("assets/images/font.png"), TextureLoad::NO_MIPMAP_LINEAR
    );
    shader = load_shader("text_vertex.glsl", "text_fragment.glsl");
}

auto Text::get_proj(tmine::f32 aspect_ratio) -> glm::mat4 {
    return glm::ortho(-aspect_ratio, aspect_ratio, -1.0f, 1.0f, 0.0f, 100.0f);
}

Text::Text(std::string text, glm::vec2 position, float fontSize)
: text{std::move(text)}
, mesh{{}, Text::VERTEX_ATTRIBUTE_SIZES, Primitive::Triangles} {
    /* init */
    this->position = position;
    this->fontSize = fontSize;

    /* Calculating length */
    float length = 0.0;
    for (unsigned int i = 0; i < this->text.size(); i++) {
        auto& curr = chars.Chars[(usize) this->text[i]];
        float sizeW = chars.Width;

        length += (curr.XAdvance) / sizeW * fontSize;
    }

    cursorPointer = -length / 2.0f;

    auto& buffer = this->mesh.get_buffer();

    /* Load all quads to array */
    for (unsigned int i = 0; i < this->text.size(); i++) {
        auto& curr = chars.Chars[(usize) this->text[i]];
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
            n_vertices, cursorPointer,
            ((lh - curr.YOffset) - h - lh / 2.0f) / sizeH, rw / sizeW,
            rh / sizeH, x / sizeW, y / sizeH, w / sizeW, h / sizeH, 1.0f, 1.0f,
            1.0f
        );

        cursorPointer += (curr.XAdvance) / sizeW * fontSize;
    }
}

void Text::render(f32 aspect_ratio) {
    if (io.just_pressed(Key::R)) {
        shader = load_shader("text_vertex.glsl", "text_fragment.glsl");
        reload();
    }

    shader.bind();
    fontTex.bind(0);

    auto const proj = Text::get_proj(aspect_ratio);
    model = glm::translate(
        glm::mat4(1.0f), glm::vec3(position.x, position.y, 0.0f)
    );

    shader.uniform_mat4("modelProj", proj * model);

    mesh.reload_buffer();
    mesh.draw();
}

void Text::reload() {
    float length = 0.0;
    for (unsigned int i = 0; i < text.size(); i++) {
        auto& curr = chars.Chars[(usize) text[i]];
        float sizeW = chars.Width;

        length += (curr.XAdvance) / sizeW * fontSize;
    }

    cursorPointer = -length / 2.0f;
    auto& buffer = this->mesh.get_buffer();

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
    char constexpr FONT_FILE_NAME[] = "assets/fonts/font.fnt";
    Stream.open(FONT_FILE_NAME);

    if (!Stream.is_open()) {
        throw std::runtime_error(
            fmt::format("failed to open font file '{}'", FONT_FILE_NAME)
        );
    }

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
